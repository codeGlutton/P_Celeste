#include "Pch.h"
#include "DialogueWidget.h"

#include "Engine.h"
#include "Device.h"
#include "World/World.h"
#include "World/WorldManager.h"
#include "World/WorldAssetManager.h"
#include "World/CameraManager.h"
#include "World/Input.h"

#include "UI/Image.h"
#include "UI/TextBlock.h"

#include "Dialogue/DialogueData.h"
#include "Dialogue/Conversable.h"

bool CDialogueWidget::Init()
{
	CWidgetContainer::Init();

	FResolution	RS = CDevice::GetInst()->GetResolution();
	float ZoomRate = mWorld.lock()->GetCameraManager().lock()->GetMainCameraZoomRate();

	FVector3 PanelSize = FVector3(1.f, 1.f, 1.f);
	PanelSize.x = RS.Width * ZoomRate * mDialoguePanelRateX;
	PanelSize.y = RS.Height * ZoomRate * mDialoguePanelRateY;

	FVector3 PanelOffset = FVector3(1.f, 1.f, 1.f);
	PanelOffset.x = RS.Width * ZoomRate * 0.5f;
	PanelOffset.y = RS.Height * ZoomRate * mDialoguePanelOffsetRateY + PanelSize.y * 0.5f;

	InitDialogue(RS, ZoomRate, PanelSize, PanelOffset);
	InitBinding();

	SetEnable(false);

	return true;
}

void CDialogueWidget::Update(float DeltaTime)
{
	CWidgetContainer::Update(DeltaTime);

	std::shared_ptr<FDialogueContext> Context = mCurContext.lock();
	if (Context == nullptr)
	{
		return;
	}

	if (mAnimState != EDialogueWidgetAnimState::None)
	{
		std::shared_ptr<CWidgetContainer> RootContainer = mRootContainer.lock();
		if (RootContainer == nullptr)
		{
			return;
		}

		mNextImage.lock()->SetAnimPos(0.f, 0.f);
		mNextArrowAccAnimTime = 0.f;

		mAccAnimTime += (mAnimState == EDialogueWidgetAnimState::FadeOut) ? -DeltaTime : DeltaTime;
		if (mAnimState == EDialogueWidgetAnimState::FadeOut)
		{
			if (mAccAnimTime <= 0.f)
			{
				if (mCurContextIndex >= Context->mDialogueDatas.size())
				{
					mAccAnimTime = 0.f;
					mAnimState = EDialogueWidgetAnimState::None;
					OnEndDialogue();
				}
				else
				{
					mAccAnimTime *= -1.f;
					mAnimState = EDialogueWidgetAnimState::FadeIn;
					OnNextDialogue();
				}
			}
		}
		if (mAnimState == EDialogueWidgetAnimState::FadeIn)
		{
			if (mAccAnimTime >= mMaxAnimTime)
			{
				mAccAnimTime = mMaxAnimTime;
				mAnimState = EDialogueWidgetAnimState::None;
			}
		}

		RootContainer->SetAnimScale(1.f, mAccAnimTime / mMaxAnimTime);
	}
	else
	{
		{
			mNextArrowAccAnimTime += DeltaTime;
			while (mNextArrowAccAnimTime >= mNextArrowMaxAnimTime)
			{
				mNextArrowAccAnimTime -= mNextArrowMaxAnimTime;
			}

			std::shared_ptr<CImage> NextImg = mNextImage.lock();
			float NextArrowAnimPos = MathUtils::BezierQuad(
				0.f,
				NextImg->GetSize().y * mNextImageAnimOffsetRateY,
				0.f,
				mNextArrowAccAnimTime / mNextArrowMaxAnimTime
			);
			NextImg->SetAnimPos(0.f, NextArrowAnimPos);
		}
		{
			FDialogueData& CurData = Context->mDialogueDatas[mCurContextIndex];
			std::shared_ptr<IConversable> Target;
			if (CurData.mTurn == EDialogueTurn::Player)
			{
				Target = mPlayer.lock();
			}
			else
			{
				Target = mOther.lock();
			}

			std::shared_ptr<CImage> Portrait = mPortraitImage.lock();
			std::shared_ptr<FDialoguePortraitBrush> PortraitBrush;
			if (mIsPortraitAnimLoop == true)
			{
				PortraitBrush = Target->GetDialoguePortraitBrush(CurData.mLoopEmotionName);
			}
			else
			{
				PortraitBrush = Target->GetDialoguePortraitBrush(CurData.mStartEmotionName);
				if (PortraitBrush->mIsEnd == true)
				{
					mIsPortraitAnimLoop = true;
					PortraitBrush = Target->GetDialoguePortraitBrush(CurData.mLoopEmotionName);
					PortraitBrush->Reset();
					PortraitBrush->SetRepeatCount(0);
					Portrait->SetTint(PortraitBrush->mTint);
				}
			}
			std::string SoundName = PortraitBrush->GetCurSoundName();
			if (SoundName.empty() == false)
			{
				mWorld.lock()->GetWorldAssetManager().lock()->SoundPlay(SoundName);
			}
			PortraitBrush->PlayAnimation(DeltaTime);
			Portrait->SetTexture(PortraitBrush->GetCurTextureName());
		}
	}
}

void CDialogueWidget::StartDialogue(std::shared_ptr<IConversable> Player, std::shared_ptr<IConversable> Other)
{
	std::shared_ptr<FDialogueContext> Context = Other->GetDialogueContext();
	if (Context == nullptr)
	{
		return;
	}
	mCurContext = Context;
	mCurContextIndex = -1;

	mPlayer = Player;
	mOther = Other;

	MoveNextDialogue();
}

void CDialogueWidget::InitDialogue(const FResolution& RS, float ZoomRate, const FVector3& PanelSize, const FVector3& PanelOffset)
{
	const FVector3 PanelContentSize = FVector3(
		PanelSize.x * (1.f - mDialoguePaddingRateX),
		PanelSize.y * (1.f - mDialoguePaddingRateY),
		1.f
	);
	const float PanelPortraitMinSize = std::min<float>(
		PanelContentSize.x * mPortraitRateX,
		PanelContentSize.y
	);
	const FVector3 PanelPortraitSize = FVector3(
		PanelPortraitMinSize,
		PanelPortraitMinSize,
		1.f
	);
	const FVector3 PanelTextSize = FVector3(
		PanelContentSize.x * (1.f - mDialogueTextPaddingRateX) - PanelPortraitMinSize,
		PanelContentSize.y * (1.f - mDialogueTextPaddingRateY),
		1.f
	);
	const FVector3 PanelNextImgSize = FVector3(
		PanelTextSize.y * mNextImgRateY * 2.f,
		PanelTextSize.y * mNextImgRateY,
		1.f
	);

	const float FontSize = std::min<float>(
		PanelTextSize.x,
		PanelTextSize.y
	) * 0.12f;

	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("Container", 0).lock();
	{
		mRootContainer = Container;

		Container->SetUseInheritedColor(false);
		Container->SetPos(PanelOffset);
		Container->SetPivot(0.5f, 0.5f);
		Container->SetSize(PanelSize);
		Container->SetAnimScale(1.f, 0.f);

		std::shared_ptr<CImage> PanelImage = Container->CreateWidget<CImage>("PanelImage", 0).lock();
		{
			mPanelImage = PanelImage;

			PanelImage->SetUseInheritedColor(false);
			PanelImage->SetPos(0.f, 0.f);
			PanelImage->SetPivot(0.5f, 0.5f);
			PanelImage->SetSize(PanelSize);
		}
		std::shared_ptr<CWidgetContainer> ContentContainer = Container->CreateWidget<CWidgetContainer>("ContentContainer", 1).lock();
		{
			ContentContainer->SetUseInheritedColor(false);
			ContentContainer->SetPos(0.f, 0.f);
			ContentContainer->SetPivot(0.5f, 0.5f);
			ContentContainer->SetSize(PanelContentSize);
		}
		std::shared_ptr<CImage> PortraitImage = ContentContainer->CreateWidget<CImage>("PortraitImage", 0).lock();
		{
			mPortraitImage = PortraitImage;

			PortraitImage->SetUseInheritedColor(false);
			PortraitImage->SetPos((PanelPortraitSize.x - PanelContentSize.x) * 0.5f, 0.f);
			PortraitImage->SetPivot(0.5f, 0.5f);
			PortraitImage->SetSize(PanelPortraitSize);
		}
		std::shared_ptr<CTextBlock> DialogueText = ContentContainer->CreateWidget<CTextBlock>("DialogueText", 0).lock();
		{
			mDialogueText = DialogueText;

			DialogueText->SetUseInheritedColor(false);
			DialogueText->SetPos(PortraitImage->GetSize().x * 0.5f, 0.f);
			DialogueText->SetPivot(0.5f, 0.5f);
			DialogueText->SetSize(PanelTextSize);

			DialogueText->SetTextColor(FVector4::White);

			DialogueText->SetFont("EngineLight");
			DialogueText->SetFontSize(FontSize);

			DialogueText->SetAlignH(ETextAlignH::Center);
			DialogueText->SetAlignV(ETextAlignV::Middle);
		}
		std::shared_ptr<CImage> NextImage = ContentContainer->CreateWidget<CImage>("NextImage", 1).lock();
		{
			mNextImage = NextImage;

			NextImage->SetUseInheritedColor(false);
			NextImage->SetPos((PanelTextSize.x - PanelNextImgSize.x) * 0.5f, 0.f);
			NextImage->SetPivot(0.5f, PanelTextSize.y * -0.25f / PanelNextImgSize.y);
			NextImage->SetSize(PanelNextImgSize);

			NextImage->SetTexture("T_DialogPoint");
		}
	}
}

void CDialogueWidget::InitBinding()
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CInput> InputMgr = World->GetInput().lock();
	
	InputMgr->AddBindKey("Dialogue", 'X');
	InputMgr->SetBindFunction<CDialogueWidget>("Dialogue", EInputType::Press, this, &CDialogueWidget::MoveNextDialogue);
}

void CDialogueWidget::MoveNextDialogue()
{
	std::shared_ptr<FDialogueContext> Context = mCurContext.lock();
	if (Context == nullptr || mAnimState != EDialogueWidgetAnimState::None)
	{
		return;
	}
	if (Context->mDialogueDatas.empty() == true || mCurContextIndex >= static_cast<int32>(Context->mDialogueDatas.size()))
	{
		return;
	}

	++mCurContextIndex;
	if (mCurContextIndex == 0)
	{
		mAccAnimTime = 0.f;
		mAnimState = EDialogueWidgetAnimState::FadeIn;
		OnStartDialogue();
	}
	else
	{
		mAccAnimTime = mMaxAnimTime;
		mAnimState = EDialogueWidgetAnimState::FadeOut;
	}
}

void CDialogueWidget::OnStartDialogue()
{
	SetEnable(true);

	std::shared_ptr<FDialogueContext> Context = mCurContext.lock();
	FDialogueData& CurData = Context->mDialogueDatas[mCurContextIndex];

	std::shared_ptr<IConversable> Target;
	if (CurData.mTurn == EDialogueTurn::Player)
	{
		Target = mPlayer.lock();
	}
	else
	{
		Target = mOther.lock();
	}

	std::shared_ptr<CImage> Panel = mPanelImage.lock();
	Panel->SetTexture(Target->GetDialoguePanelBrush()->mPanelTexture);
	Panel->SetTint(Target->GetDialoguePanelBrush()->mTint);

	std::shared_ptr<CImage> Portrait = mPortraitImage.lock();
	std::shared_ptr<FDialoguePortraitBrush> PortraitBrush;
	if (CurData.mStartEmotionName.empty() == true)
	{
		mIsPortraitAnimLoop = true;
		PortraitBrush = Target->GetDialoguePortraitBrush(CurData.mLoopEmotionName);
	}
	else
	{
		mIsPortraitAnimLoop = false;
		PortraitBrush = Target->GetDialoguePortraitBrush(CurData.mStartEmotionName);
	}
	PortraitBrush->Reset();
	PortraitBrush->SetRepeatCount(CurData.mText.size());
	Portrait->SetTexture(PortraitBrush->GetCurTextureName());
	Portrait->SetTint(PortraitBrush->mTint);

	std::shared_ptr<CTextBlock> Text = mDialogueText.lock();
	Text->SetText(CurData.mText.c_str());
}

void CDialogueWidget::OnNextDialogue()
{
	std::shared_ptr<FDialogueContext> Context = mCurContext.lock();

	FDialogueData& PreData = Context->mDialogueDatas[mCurContextIndex - 1];
	FDialogueData& CurData = Context->mDialogueDatas[mCurContextIndex];
	if (CurData.IsNeedChangeAnim(PreData) == true)
	{
		if (PreData.mOnEnd != nullptr)
		{
			PreData.mOnEnd();
		}

		std::shared_ptr<IConversable> Target;
		if (CurData.mTurn == EDialogueTurn::Player)
		{
			Target = mPlayer.lock();
		}
		else
		{
			Target = mOther.lock();
		}

		std::shared_ptr<CImage> Panel = mPanelImage.lock();
		Panel->SetTexture(Target->GetDialoguePanelBrush()->mPanelTexture);
		Panel->SetTint(Target->GetDialoguePanelBrush()->mTint);

		std::shared_ptr<CImage> Portrait = mPortraitImage.lock();
		std::shared_ptr<FDialoguePortraitBrush> PortraitBrush;
		if (CurData.mStartEmotionName.empty() == true)
		{
			mIsPortraitAnimLoop = true;
			PortraitBrush = Target->GetDialoguePortraitBrush(CurData.mLoopEmotionName);
		}
		else
		{
			mIsPortraitAnimLoop = false;
			PortraitBrush = Target->GetDialoguePortraitBrush(CurData.mStartEmotionName);
		}
		PortraitBrush->Reset();
		PortraitBrush->SetRepeatCount(CurData.mText.size());
		Portrait->SetTexture(PortraitBrush->GetCurTextureName());
		Portrait->SetTint(PortraitBrush->mTint);

		std::shared_ptr<CTextBlock> Text = mDialogueText.lock();
		Text->SetText(CurData.mText.c_str());
	}
}

void CDialogueWidget::OnEndDialogue()
{
	std::shared_ptr<FDialogueContext> Context = mCurContext.lock();
	FDialogueData& PreData = Context->mDialogueDatas[mCurContextIndex - 1];
	if (PreData.mOnEnd != nullptr)
	{
		PreData.mOnEnd();
	}

	mCurContext.reset();
	mPlayer.reset();
	mOther.reset();

	mCurContextIndex = -1;

	mOnEndDialogue();
	mOnEndDialogue = nullptr;

	SetEnable(false);
}
