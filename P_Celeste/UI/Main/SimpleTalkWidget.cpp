#include "Pch.h"
#include "SimpleTalkWidget.h"

#include "Engine.h"
#include "Device.h"
#include "World/World.h"
#include "World/WorldManager.h"
#include "World/CameraManager.h"

#include "UI/Image.h"
#include "UI/TextBlock.h"

bool CSimpleTalkWidget::Init()
{
	CWidgetContainer::Init();

	CWidgetContainer::Init();

	FResolution	RS = CDevice::GetInst()->GetResolution();
	float ZoomRate = mWorld.lock()->GetCameraManager().lock()->GetMainCameraZoomRate();

	mTextBoxMaxSize.x = RS.Width * ZoomRate * mTalkBoxRateX;
	mTextBoxMaxSize.y = RS.Height * ZoomRate * mTalkBoxRateY;
	mTextBoxMaxPointSize.x = RS.Width * ZoomRate * mTalkBoxPointRateX;
	mTextBoxMaxPointSize.y = RS.Height * ZoomRate * mTalkBoxPointRateX * 0.5f;

	InitText(RS, ZoomRate);

	return true;
}

void CSimpleTalkWidget::Update(float DeltaTime)
{
	CWidgetContainer::Update(DeltaTime);

	std::shared_ptr<CWidgetContainer> Container = mRootContainer.lock();
	if (Container == nullptr)
	{
		return;
	}

	bool NeedToChange = false;
	if (mTalkState == ESimpleTalkState::Dirty)
	{
		const int32 TextCount = mTalkMainText.lock()->GetTextCount();
		if (TextCount > 0)
		{
			mTalkState = ESimpleTalkState::FadeOut;
		}
		else
		{
			mTalkState = ESimpleTalkState::FadeIn;
			NeedToChange = true;
		}
		mAccFadeTime = 0.f;
	}
	else if (mTalkState != ESimpleTalkState::Finish)
	{
		mAccFadeTime += DeltaTime;
		float FadeRate = std::min<float>(1.f, mAccFadeTime / mMaxFadeTime);
		float AnimScaleX;
		if (mTalkState == ESimpleTalkState::FadeOut)
		{
			AnimScaleX = 1.f - FadeRate;
			if (FadeRate >= 1.f)
			{
				mAccFadeTime -= mMaxFadeTime;
				FadeRate = std::min<float>(1.f, mAccFadeTime / mMaxFadeTime);
				NeedToChange = true;

				const int32 TextCount = mNextMainText.size();
				mTalkState = (TextCount > 0) ? ESimpleTalkState::FadeIn : ESimpleTalkState::Finish;
			}
		}
		if (mTalkState == ESimpleTalkState::FadeIn)
		{
			AnimScaleX = FadeRate;
			if (FadeRate >= 1.f)
			{
				mAccFadeTime = 0.f;
				mTalkState = ESimpleTalkState::Finish;
			}
		}

		Container->SetAnimScale(AnimScaleX, 1.f);
	}

	if (NeedToChange == true)
	{
		mTalkMainText.lock()->SetText(mNextMainText.c_str());
		mTalkContentText.lock()->SetText(mNextContentText.c_str());
		if (mNextSubTextureName.empty() == true)
		{
			mSubImage.lock()->SetTexture(std::weak_ptr<CTexture>());
		}
		else
		{
			mSubImage.lock()->SetTexture(mNextSubTextureName);
		}

		mNextMainText.clear();
		mNextContentText.clear();
		mNextSubTextureName.clear();
	}
}

void CSimpleTalkWidget::Render()
{
	CWidgetContainer::Render();
}

void CSimpleTalkWidget::InitText(const FResolution& RS, float ZoomRate)
{
	const float OutlineSize = std::max<float>(mTextBoxMaxSize.x * mTalkBoxOutlinePaddingRate, mTextBoxMaxSize.y * mTalkBoxOutlinePaddingRate);
	const FVector3 InnerBoxMaxSize = FVector2(mTextBoxMaxSize.x - OutlineSize, mTextBoxMaxSize.y - OutlineSize);

	const FVector2 TextPaddingSize = FVector2(mTextBoxMaxSize.x * mTalkBoxPaddingRate, mTextBoxMaxSize.y * mTalkBoxPaddingRate);
	const FVector3 TextBoxMaxSize = FVector2(mTextBoxMaxSize.x - OutlineSize - TextPaddingSize.x, mTextBoxMaxSize.y - OutlineSize - TextPaddingSize.y);
	const float FontSize = std::min<float>(
		TextBoxMaxSize.x,
		TextBoxMaxSize.y
	) * 0.325f;

	const float SubImageMaxSize = std::min<float>(mTextBoxMaxSize.x * mTalkSubImageRateX, mTextBoxMaxSize.y * mTalkSubImageRateX);

	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("Container").lock();
	{
		mRootContainer = Container;

		Container->SetPos(0.f, 0.f);
		Container->SetPivot(0.5f, 0.5f);
		Container->SetSize(mTextBoxMaxSize);
		Container->SetOpacity(0.f);
		Container->SetAnimScale(0.f, 1.f);

		std::shared_ptr<CImage> OutlineBox = Container->CreateWidget<CImage>("OutlineBox", 0).lock();
		{
			OutlineBox->SetUseInheritedColor(false);
			OutlineBox->SetPos(0.f, 0.f);
			OutlineBox->SetPivot(0.5f, 0.5f);
			OutlineBox->SetSize(mTextBoxMaxSize);
			OutlineBox->SetWidgetColor(mOutlineBoxColor);
		}
		std::shared_ptr<CImage> Box = Container->CreateWidget<CImage>("Box", 1).lock();
		{
			Box->SetUseInheritedColor(false);
			Box->SetPos(0.f, 0.f);
			Box->SetPivot(0.5f, 0.5f);
			Box->SetSize(InnerBoxMaxSize);
			Box->SetWidgetColor(mBoxColor);
		}
		std::shared_ptr<CImage> OutlineBoxPoint = Container->CreateWidget<CImage>("OutlineBoxPoint", 0).lock();
		{
			OutlineBoxPoint->SetUseInheritedColor(false);
			OutlineBoxPoint->SetPos(0.f, InnerBoxMaxSize.y * 0.5f);
			OutlineBoxPoint->SetPivot(0.5f, 0.f);
			OutlineBoxPoint->SetSize(mTextBoxMaxPointSize);
			OutlineBoxPoint->SetWidgetColor(mOutlineBoxColor);

			OutlineBoxPoint->SetTexture("T_DialogPoint");
		}
		std::shared_ptr<CImage> BoxPoint = Container->CreateWidget<CImage>("BoxPoint", 1).lock();
		{
			BoxPoint->SetUseInheritedColor(false);
			BoxPoint->SetPos(0.f, InnerBoxMaxSize.y * 0.5f - OutlineSize * 0.5f);
			BoxPoint->SetPivot(0.5f, 0.f);
			BoxPoint->SetSize(mTextBoxMaxPointSize);
			BoxPoint->SetWidgetColor(mBoxColor);

			BoxPoint->SetTexture("T_DialogPoint");
		}
		std::shared_ptr<CTextBlock> MainText = Container->CreateWidget<CTextBlock>("MainText", 2).lock();
		{
			mTalkMainText = MainText;

			MainText->SetUseInheritedColor(false);
			MainText->SetPos(0.f, -TextBoxMaxSize.y / 4.f);
			MainText->SetPivot(0.5f, 0.5f);
			MainText->SetSize(TextBoxMaxSize.x, TextBoxMaxSize.y / 2.f);

			MainText->SetTextColor(mMainTextColor);
			MainText->SetText(TEXT(""));

			MainText->SetFont("EngineLight");
			MainText->SetFontSize(FontSize);

			MainText->SetAlignH(ETextAlignH::Center);
			MainText->SetAlignV(ETextAlignV::Middle);
		}
		std::shared_ptr<CTextBlock> ContentText = Container->CreateWidget<CTextBlock>("ContentText", 2).lock();
		{
			mTalkContentText = ContentText;

			ContentText->SetUseInheritedColor(false);
			ContentText->SetPos(0.f, TextBoxMaxSize.y / 4.f);
			ContentText->SetPivot(0.5f, 0.5f);
			ContentText->SetSize(TextBoxMaxSize.x, TextBoxMaxSize.y / 2.f);

			ContentText->SetTextColor(mContentTextColor);
			ContentText->SetText(TEXT(""));

			ContentText->SetFont("EngineLight");
			ContentText->SetFontSize(FontSize);

			ContentText->EnableShadow(true);
			ContentText->SetShadowTextColor(mContentTextShadowColor);
			ContentText->SetShadowOffset(0.f, FontSize * 0.1f);

			ContentText->SetAlignH(ETextAlignH::Left);
			ContentText->SetAlignV(ETextAlignV::Middle);
		}
		std::shared_ptr<CImage> SubImage = Container->CreateWidget<CImage>("SubImage", 2).lock();
		{
			mSubImage = SubImage;

			SubImage->SetUseInheritedColor(false);
			SubImage->SetPos(0.f, TextBoxMaxSize.y / 4.f);
			SubImage->SetPivot(-TextBoxMaxSize.x / SubImageMaxSize * 0.3f, 0.5f);
			SubImage->SetSize(SubImageMaxSize, SubImageMaxSize);
		}
	}
}

const FVector3& CSimpleTalkWidget::GetCurTextBoxSize() const
{
	std::shared_ptr<CWidgetContainer> Container = mRootContainer.lock();
	if (Container == nullptr)
	{
		return FVector3::Zero;
	}
	return Container->GetAnimScale() * mTextBoxMaxSize;
}

const FVector3& CSimpleTalkWidget::GetMaxTextBoxSize() const
{
	return mTextBoxMaxSize;
}

const FVector3& CSimpleTalkWidget::GetMaxTextPointSize() const
{
	return mTextBoxMaxPointSize;
}

void CSimpleTalkWidget::ChangeText(const std::wstring& MainText, const std::wstring& ContentText)
{
	if (IsChangingToText(MainText, ContentText) == true)
	{
		return;
	}

	mNextMainText = MainText;
	mNextContentText = ContentText;

	if (mTalkState == ESimpleTalkState::Dirty || mTalkState == ESimpleTalkState::FadeOut)
	{
		return;
	}
	mTalkState = ESimpleTalkState::Dirty;
}

void CSimpleTalkWidget::ChangeText(const std::wstring& MainText, const std::wstring& ContentText, const std::string& TextureName)
{
	mNextSubTextureName = TextureName;
	ChangeText(MainText, ContentText);
}

bool CSimpleTalkWidget::IsChangingToText(const std::wstring& MainText, const std::wstring& ContentText) const
{
	if (mTalkState == ESimpleTalkState::FadeIn || mTalkState == ESimpleTalkState::Finish)
	{
		return mTalkMainText.lock()->GetText() == MainText && mTalkContentText.lock()->GetText() == ContentText;
	}
	return mNextMainText == MainText && mNextContentText == ContentText;
}
