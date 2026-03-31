#include "Pch.h"
#include "SoliloquyWidget.h"

#include "Engine.h"
#include "Device.h"
#include "World/World.h"
#include "World/WorldManager.h"
#include "World/CameraManager.h"
#include "World/Input.h"

#include "UI/Image.h"
#include "UI/TextBlock.h"

bool CSoliloquyWidget::Init()
{
	CWidgetContainer::Init();

	FResolution	RS = CDevice::GetInst()->GetResolution();
	float ZoomRate = mWorld.lock()->GetCameraManager().lock()->GetMainCameraZoomRate();

	InitSoliloquy(RS, ZoomRate);
	InitBinding();

	SetEnable(false);

	return true;
}

void CSoliloquyWidget::Update(float DeltaTime)
{
	CWidgetContainer::Update(DeltaTime);

	if (mSceneAnimState != ESoliloquyWidgetAnimState::None)
	{
		std::shared_ptr<CWidgetContainer> RootContainer = mRootContainer.lock();
		if (RootContainer == nullptr)
		{
			return;
		}

		mMainText.lock()->SetTextColor(FVector4(1.f, 1.f, 1.f, 0.f));

		mAccSwapSceneAnimTime += (mSceneAnimState == ESoliloquyWidgetAnimState::FadeOut) ? -DeltaTime : DeltaTime;
		if (mSceneAnimState == ESoliloquyWidgetAnimState::FadeOut)
		{
			if (mAccSwapSceneAnimTime <= 0.f)
			{
				mAccSwapSceneAnimTime = 0.f;
				mSceneAnimState = ESoliloquyWidgetAnimState::None;
				OnEndSoliloquy();
			}
		}
		else
		{
			if (mAccSwapSceneAnimTime >= mMaxSwapSceneAnimTime)
			{
				mAccSwapSceneAnimTime = mMaxSwapSceneAnimTime;
				mSceneAnimState = ESoliloquyWidgetAnimState::None;
				mTextAnimState = ESoliloquyWidgetAnimState::FadeIn;
			}
		}

		RootContainer->SetWidgetChildColor(FVector4(1.f, 1.f, 1.f, mAccSwapSceneAnimTime / mMaxSwapSceneAnimTime));
	}
	else if (mTextAnimState != ESoliloquyWidgetAnimState::None)
	{
		std::shared_ptr<CTextBlock> MainTextBlock = mMainText.lock();
		if (MainTextBlock == nullptr)
		{
			return;
		}
		const float MaxTextAnimOffsetY = mMainTextFadeRateY * MainTextBlock->GetFontSize();

		mAccTextAnimTime += (mTextAnimState == ESoliloquyWidgetAnimState::FadeOut) ? -DeltaTime : DeltaTime;
		if (mTextAnimState == ESoliloquyWidgetAnimState::FadeOut)
		{
			if (mAccTextAnimTime <= 0.f)
			{
				if (mCurTextDataIndex >= mShowTextDatas.size())
				{
					mAccTextAnimTime = 0.f;
					mTextAnimState = ESoliloquyWidgetAnimState::None;
					mSceneAnimState = ESoliloquyWidgetAnimState::FadeOut;
				}
				else
				{
					mAccTextAnimTime *= -1.f;
					mTextAnimState = ESoliloquyWidgetAnimState::FadeIn;
					OnNextSoliloquy();
				}
			}
			MainTextBlock->SetAnimPos(0.f, MaxTextAnimOffsetY * (1.f - mAccTextAnimTime / mMaxTextAnimTime));
		}
		if (mTextAnimState == ESoliloquyWidgetAnimState::FadeIn)
		{
			if (mAccTextAnimTime >= mMaxTextAnimTime)
			{
				mAccTextAnimTime = mMaxTextAnimTime;
				mTextAnimState = ESoliloquyWidgetAnimState::None;
			}
			MainTextBlock->SetAnimPos(0.f, MaxTextAnimOffsetY * (mAccTextAnimTime / mMaxTextAnimTime - 1.f));
		}

		MainTextBlock->SetTextColor(FVector4(1.f, 1.f, 1.f, mAccTextAnimTime / mMaxTextAnimTime));
	}
}

void CSoliloquyWidget::StartSoliloquy(const std::vector<std::wstring>& TextDatas)
{
	mShowTextDatas = TextDatas;
	mCurTextDataIndex = -1;

	MoveNextSoliloquy();
}

void CSoliloquyWidget::InitSoliloquy(const FResolution& RS, float ZoomRate)
{
	const FVector3 PanelSize = FVector3(
		RS.Width * ZoomRate,
		RS.Height * ZoomRate,
		1.f
	);
	const float FontSize = PanelSize.x * mMainFontRateX;

	std::shared_ptr<CImage> BackImage = CreateWidget<CImage>("BackImage", 0).lock();
	{
		BackImage->SetUseInheritedColor(false);
		BackImage->SetWidgetColor(FVector4::Black);
		BackImage->SetPos(0.f, 0.f);
		BackImage->SetPivot(0.f, 0.f);
		BackImage->SetSize(PanelSize);
	}
	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("Container", 1).lock();
	{
		mRootContainer = Container;

		Container->SetUseInheritedColor(false);
		Container->SetWidgetChildColor(FVector4::White);
		Container->SetPos(0.f, 0.f);
		Container->SetPivot(0.f, 0.f);
		Container->SetSize(PanelSize);

		std::shared_ptr<CImage> PanelImage = Container->CreateWidget<CImage>("PanelImage", 0).lock();
		{
			mPanelImage = PanelImage;

			PanelImage->SetUseInheritedColor(true);
			PanelImage->SetWidgetColor(FVector4(1.f, 1.f, 1.f, 0.2f));
			PanelImage->SetPos(0.f, 0.f);
			PanelImage->SetPivot(0.f, 0.f);
			PanelImage->SetSize(PanelSize);
			PanelImage->SetTexture("T_SoliloquyPanel");
		}
		std::shared_ptr<CTextBlock> SoliloquyText = Container->CreateWidget<CTextBlock>("SoliloquyText", 0).lock();
		{
			mMainText = SoliloquyText;

			SoliloquyText->SetUseInheritedColor(true);
			SoliloquyText->SetPos(0.f, 0.f);
			SoliloquyText->SetPivot(0.f, 0.f);
			SoliloquyText->SetSize(PanelSize);

			SoliloquyText->SetTextColor(FVector4::White);

			SoliloquyText->SetFont("EngineLight");
			SoliloquyText->SetFontSize(FontSize);

			SoliloquyText->SetAlignH(ETextAlignH::Center);
			SoliloquyText->SetAlignV(ETextAlignV::Middle);
		}
	}
}

void CSoliloquyWidget::InitBinding()
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CInput> InputMgr = World->GetInput().lock();

	InputMgr->AddBindKey("Soliloquy", 'X');
	InputMgr->SetBindFunction<CSoliloquyWidget>("Soliloquy", EInputType::Press, this, &CSoliloquyWidget::MoveNextSoliloquy);
}

void CSoliloquyWidget::MoveNextSoliloquy()
{
	if (mShowTextDatas.empty() == true || mCurTextDataIndex >= static_cast<int32>(mShowTextDatas.size()))
	{
		return;
	}

	++mCurTextDataIndex;
	mCurTextDataIndex = std::min<int32>(mCurTextDataIndex, mShowTextDatas.size());
	if (mCurTextDataIndex == 0)
	{
		mAccSwapSceneAnimTime = mAccTextAnimTime = 0.f;
		mSceneAnimState = ESoliloquyWidgetAnimState::FadeIn;
		OnStartSoliloquy();
	}
	else
	{
		mAccTextAnimTime = mMaxTextAnimTime;
		mTextAnimState = ESoliloquyWidgetAnimState::FadeOut;
	}
}

void CSoliloquyWidget::OnStartSoliloquy()
{
	SetEnable(true);
	mMainText.lock()->SetText(mShowTextDatas.front().c_str());
}

void CSoliloquyWidget::OnNextSoliloquy()
{
	mMainText.lock()->SetText(mShowTextDatas[mCurTextDataIndex].c_str());
}

void CSoliloquyWidget::OnEndSoliloquy()
{
	mShowTextDatas.clear();
	mCurTextDataIndex = -1;

	mOnEndSoliloquy();
	mOnEndSoliloquy = nullptr;

	SetEnable(false);
}
