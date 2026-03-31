#include "Pch.h"
#include "HairEditorWidget.h"

#include "World/World.h"

#include "Editor/HairEditorObject.h"

#include "UI/Image.h"
#include "UI/Button.h"
#include "UI/TextBlock.h"

#include "Asset/Texture/Texture.h"
#include "Animation/HairMetaData.h"

#include "Device.h"

bool CHairEditorWidget::Init()
{
	CWidgetContainer::Init();

	mMetaDataFilePath = TEXT("Player/") + std::wstring(mMetaDataFolderName) + TEXT(".meta");
	mDisplayTextureFilePaths.reserve(mMetaDataFrameCount);
	for (int32 i = 0; i < mMetaDataFrameCount; ++i)
	{
		std::wstring DisplayTextureFilePath = 
			TEXT("Player/") + std::wstring(mTextureFolderName) + TEXT("/f") + std::to_wstring(i + mMetaDataStartIndex) + TEXT(".png");
		mDisplayTextureFilePaths.push_back(DisplayTextureFilePath);
	}

	FResolution	RS = CDevice::GetInst()->GetResolution();

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	WorldAssetMgr->LoadTexture("T_HairEditTexture_0", mDisplayTextureFilePaths[0].c_str());
	mCurTexture = WorldAssetMgr->FindTexture("T_HairEditTexture_0").lock();

	FVector3 StartPos = FVector3::Zero;
	InitFirstLineText(RS, StartPos);

	StartPos.y += RS.Height * (mTextSizeRateY + mPaddingRateY);
	InitSecondLineButtons(RS, StartPos);

	StartPos.y += RS.Height * (mAllButtonSizeRateY + mPaddingRateY);
	InitThirdLineButtons(RS, StartPos);

	StartPos.y += RS.Height * (mAllButtonSizeRateY + mPaddingRateY);
	InitLastLineButtons(RS, StartPos);

	StartPos.y += RS.Height * (mAllButtonSizeRateY * 3);
	InitBackgorundImage(RS, StartPos);

	OnClickLoad();

	UpdateOffsetText();
	UpdateFrameText();
	UpdateTogleText();
	UpdateFlipText();

	return true;
}

void CHairEditorWidget::InitFirstLineText(const FResolution& RS, const FVector3& StartPos)
{
	const float PaddingSizeX = RS.Width * mPaddingRateX;
	const FVector2 FontBoxSize = { (RS.Width * mSizeRateX - PaddingSizeX) * 0.5f, RS.Height * mTextSizeRateY };
	const float FontSize = std::min<float>(FontBoxSize.x, FontBoxSize.y);

	mOffsetText = CreateWidget<CTextBlock>("OffsetText", 0).lock();
	{
		mOffsetText->SetPivot(0.f, 0.f);
		mOffsetText->SetPos(StartPos);
		mOffsetText->SetSize(FontBoxSize);

		mOffsetText->SetTextColor(FVector4::White);

		mOffsetText->SetFont("EngineLight");
		mOffsetText->SetFontSize(FontSize);

		mOffsetText->SetAlignH(ETextAlignH::Left);
		mOffsetText->SetAlignV(ETextAlignV::Middle);
	}

	mFrameText = CreateWidget<CTextBlock>("FrameText", 0).lock();
	{
		mFrameText->SetPivot(0.f, 0.f);
		mFrameText->SetPos(StartPos.x + FontBoxSize.x + PaddingSizeX, StartPos.y);
		mFrameText->SetSize(FontBoxSize);

		mFrameText->SetTextColor(FVector4::White);

		mFrameText->SetFont("EngineLight");
		mFrameText->SetFontSize(FontSize);

		mFrameText->SetAlignH(ETextAlignH::Left);
		mFrameText->SetAlignV(ETextAlignV::Middle);
	}
}

void CHairEditorWidget::InitSecondLineButtons(const FResolution& RS, const FVector3& StartPos)
{
	const float PaddingSizeX = RS.Width * mPaddingRateX;
	const FVector2 ButtonSize = { (RS.Width * mSizeRateX - PaddingSizeX) * 0.5f, RS.Height * mAllButtonSizeRateY };
	const float FontSize = std::min<float>(ButtonSize.x, ButtonSize.y);

	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("FirstContainer", 0).lock();
	Container->SetPivot(0.f, 0.f);
	Container->SetPos(StartPos);
	Container->SetSize(ButtonSize.x * 2 + PaddingSizeX, ButtonSize.y);
	{
		std::shared_ptr<CImage> BackImage = Container->CreateWidget<CImage>("FirstContainerBackImage", -1).lock();
		{
			BackImage->SetPivot(0.f, 0.f);
			BackImage->SetPos(0.f, 0.f);
			BackImage->SetSize(Container->GetSize());

			BackImage->SetWidgetColor(FVector4(0.1f, 0.1f, 0.1f, 1.f));
		}
		mSaveButton = Container->CreateWidget<CButton>("SaveButton", 0).lock();
		{
			mSaveButton->SetPivot(0.f, 0.f);
			mSaveButton->SetPos(0.f, 0.f);
			mSaveButton->SetSize(ButtonSize);

			mSaveButton->SetWidgetColor(FVector4::Black);

			mSaveButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickSave);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("SaveText", mWorld);
			mSaveButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mSaveButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("Save"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mLoadButton = Container->CreateWidget<CButton>("LoadButton", 0).lock();
		{
			mLoadButton->SetPivot(0.f, 0.f);
			mLoadButton->SetPos((ButtonSize.x + PaddingSizeX) * 1.f, 0.f);
			mLoadButton->SetSize(ButtonSize);

			mLoadButton->SetWidgetColor(FVector4::Black);

			mLoadButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickLoad);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("LoadText", mWorld);
			mLoadButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mLoadButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("Load"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CHairEditorWidget::InitThirdLineButtons(const FResolution& RS, const FVector3& StartPos)
{
	const float PaddingSizeX = RS.Width * mPaddingRateX;
	const FVector2 ButtonSize = { (RS.Width * mSizeRateX - PaddingSizeX * 2.f) / 3.f, RS.Height * mAllButtonSizeRateY };
	const float FontSize = std::min<float>(ButtonSize.x, ButtonSize.y);

	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("SecondContainer", 0).lock();
	Container->SetPivot(0.f, 0.f);
	Container->SetPos(StartPos);
	Container->SetSize(ButtonSize.x * 3 + PaddingSizeX * 2, ButtonSize.y);
	{
		std::shared_ptr<CImage> BackImage = Container->CreateWidget<CImage>("SecondContainerBackImage", -1).lock();
		{
			BackImage->SetPivot(0.f, 0.f);
			BackImage->SetPos(0.f, 0.f);
			BackImage->SetSize(Container->GetSize());

			BackImage->SetWidgetColor(FVector4(0.1f, 0.1f, 0.1f, 1.f));
		}
		mFrontBackToggleButton = Container->CreateWidget<CButton>("FrontBackToggleButton", 0).lock();
		{
			mFrontBackToggleButton->SetPivot(0.f, 0.f);
			mFrontBackToggleButton->SetPos(0.f, 0.f);
			mFrontBackToggleButton->SetSize(ButtonSize);

			mFrontBackToggleButton->SetWidgetColor(FVector4::Black);

			mFrontBackToggleButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickFrontBackToggle);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("FrontBackToggleText", mWorld);
			mFrontBackToggleButton->SetChild(Text);
			{
				mToggleText = Text;

				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mFrontBackToggleButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mPreFrameButton = Container->CreateWidget<CButton>("PreFrameButton", 0).lock();
		{
			mPreFrameButton->SetPivot(0.f, 0.f);
			mPreFrameButton->SetPos((ButtonSize.x + PaddingSizeX) * 1.f, 0.f);
			mPreFrameButton->SetSize(ButtonSize);

			mPreFrameButton->SetWidgetColor(FVector4::Black);

			mPreFrameButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickPreFrame);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("PreFrameText", mWorld);
			mPreFrameButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mPreFrameButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("<<"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mNextFrameButton = Container->CreateWidget<CButton>("NextFrameButton", 0).lock();
		{
			mNextFrameButton->SetPivot(0.f, 0.f);
			mNextFrameButton->SetPos((ButtonSize.x + PaddingSizeX) * 2.f, 0.f);
			mNextFrameButton->SetSize(ButtonSize);

			mNextFrameButton->SetWidgetColor(FVector4::Black);

			mNextFrameButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickNextFrame);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("NextFrameText", mWorld);
			mNextFrameButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mNextFrameButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT(">>"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CHairEditorWidget::InitLastLineButtons(const FResolution& RS, const FVector3& StartPos)
{
	const float PaddingSizeX = RS.Width * mPaddingRateX;
	const FVector2 ButtonSize = { (RS.Width * mSizeRateX - PaddingSizeX * 2.f) / 3.f, RS.Height * mAllButtonSizeRateY };
	const float FontSize = std::min<float>(ButtonSize.x, ButtonSize.y);

	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("LastContainer", 0).lock();
	Container->SetPivot(0.f, 0.f);
	Container->SetPos(StartPos);
	Container->SetSize(ButtonSize.x * 3 + PaddingSizeX * 2, ButtonSize.y * 3);
	{
		std::shared_ptr<CImage> BackImage = Container->CreateWidget<CImage>("LastContainerBackImage", -1).lock();
		{
			BackImage->SetPivot(0.f, 0.f);
			BackImage->SetPos(0.f, 0.f);
			BackImage->SetSize(Container->GetSize());

			BackImage->SetWidgetColor(FVector4(0.1f, 0.1f, 0.1f, 1.f));
		}
		mFlipButton = Container->CreateWidget<CButton>("FlipButton", 0).lock();
		{
			mFlipButton->SetPivot(0.f, 0.f);
			mFlipButton->SetPos((ButtonSize.x + PaddingSizeX) * 1.f, (ButtonSize.y) * 1.f);
			mFlipButton->SetSize(ButtonSize);

			mFlipButton->SetWidgetColor(FVector4::Black);

			mFlipButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickFlip);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("FlipText", mWorld);
			mFlipButton->SetChild(Text);
			{
				mFlipText = Text;

				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mFlipButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mPreCopyButton = Container->CreateWidget<CButton>("PreCopyButton", 0).lock();
		{
			mPreCopyButton->SetPivot(0.f, 0.f);
			mPreCopyButton->SetPos((ButtonSize.x + PaddingSizeX) * 2.f, 0.f);
			mPreCopyButton->SetSize(ButtonSize);

			mPreCopyButton->SetWidgetColor(FVector4::Black);

			mPreCopyButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickPreCopy);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("PreCopyText", mWorld);
			mPreCopyButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mPreCopyButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("Pre Copy"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize * 0.5f);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mUpOffsetButton = Container->CreateWidget<CButton>("UpOffsetButton", 0).lock();
		{
			mUpOffsetButton->SetPivot(0.f, 0.f);
			mUpOffsetButton->SetPos((ButtonSize.x + PaddingSizeX) * 1.f, 0.f);
			mUpOffsetButton->SetSize(ButtonSize);

			mUpOffsetButton->SetWidgetColor(FVector4::Black);

			mUpOffsetButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickMoveOffsetUp);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("UpOffsetText", mWorld);
			mUpOffsetButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mUpOffsetButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("▲"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mLeftOffsetButton = Container->CreateWidget<CButton>("LeftOffsetButton", 0).lock();
		{
			mLeftOffsetButton->SetPivot(0.f, 0.f);
			mLeftOffsetButton->SetPos(0.f, (ButtonSize.y) * 1.f);
			mLeftOffsetButton->SetSize(ButtonSize);

			mLeftOffsetButton->SetWidgetColor(FVector4::Black);

			mLeftOffsetButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickMoveOffsetLeft);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("LeftOffsetText", mWorld);
			mLeftOffsetButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mLeftOffsetButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("◀"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mRightOffsetButton = Container->CreateWidget<CButton>("RightOffsetButton", 0).lock();
		{
			mRightOffsetButton->SetPivot(0.f, 0.f);
			mRightOffsetButton->SetPos((ButtonSize.x + PaddingSizeX) * 2.f, (ButtonSize.y) * 1.f);
			mRightOffsetButton->SetSize(ButtonSize);

			mRightOffsetButton->SetWidgetColor(FVector4::Black);

			mRightOffsetButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickMoveOffsetRight);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("RightOffsetText", mWorld);
			mRightOffsetButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mRightOffsetButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("▶"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mDownOffsetButton = Container->CreateWidget<CButton>("DownOffsetButton", 0).lock();
		{
			mDownOffsetButton->SetPivot(0.f, 0.f);
			mDownOffsetButton->SetPos((ButtonSize.x + PaddingSizeX) * 1.f, (ButtonSize.y) * 2.f);
			mDownOffsetButton->SetSize(ButtonSize);

			mDownOffsetButton->SetWidgetColor(FVector4::Black);

			mDownOffsetButton->SetEventCallback(EButtonEventState::Click, this, &CHairEditorWidget::OnClickMoveOffsetDown);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("DownOffsetText", mWorld);
			mDownOffsetButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mDownOffsetButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("▼"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CHairEditorWidget::InitBackgorundImage(const FResolution& RS, const FVector3& StartPos)
{
	const FVector2 ImageSize = { RS.Width * mSizeRateX, RS.Height * (mTextSizeRateY + mAllButtonSizeRateY * 5 + mPaddingRateY * 3) };

	std::shared_ptr<CImage> BackImage = CreateWidget<CImage>("BackgroundImage", -1).lock();
	{
		BackImage->SetPivot(0.f, 0.f);
		BackImage->SetPos(0.f, 0.f);
		BackImage->SetSize(ImageSize);

		BackImage->SetWidgetColor(FVector4(0.2f, 0.2f, 0.2f, 1.f));
	}
}

void CHairEditorWidget::OnClickSave()
{
	if (mCurMetaData == nullptr)
	{
		mCurMetaData.reset(new CHairMetaData);
		mCurMetaData->SetName("A_HairEditMetaData");

		FHairFrameData mTmpFrameMetaData;
		for (int32 i = 0; i < mDisplayTextureFilePaths.size(); ++i)
		{
#ifdef _DEBUG
			mCurMetaData->SetFrameMetaData(i, &mTmpFrameMetaData);
#endif
		}
	}

	FHairFrameData NewFrameMetaData;
	NewFrameMetaData.mFrontHairOffset = mCurFrontOffset;
	NewFrameMetaData.mBackHairOffset = mCurBackOffset;
	NewFrameMetaData.mIsFlip = mCurFlip;

#ifdef _DEBUG
	mCurMetaData->SetFrameMetaData(mCurFrameIndex, &NewFrameMetaData);
#endif

	mCurMetaData->Save(mMetaDataFilePath.c_str(), "Animation");
}

void CHairEditorWidget::OnClickLoad()
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	if (WorldAssetMgr->LoadAnimMetaData<CHairMetaData>("A_HairEditMetaData", mMetaDataFilePath.c_str()) == false)
	{
		OnClickSave();
		WorldAssetMgr->LoadAnimMetaData<CHairMetaData>("A_HairEditMetaData", mMetaDataFilePath.c_str());
	}
	mCurMetaData = WorldAssetMgr->FindAnimMetaData<CHairMetaData>("A_HairEditMetaData").lock();

	if (mCurMetaData != nullptr)
	{
		const FHairFrameData* FrameMetaData = mCurMetaData->GetFrameMetaData<FHairFrameData>(mCurFrameIndex);
		if (FrameMetaData != nullptr)
		{
			mCurFrontOffset = FrameMetaData->mFrontHairOffset;
			mCurBackOffset = FrameMetaData->mBackHairOffset;
			mCurFlip = FrameMetaData->mIsFlip;
		}
	}

	if (mDisplayObject != nullptr)
	{
		mDisplayObject->SetOffset(true, mCurFrontOffset);
		mDisplayObject->SetOffset(false, mCurBackOffset);
		mDisplayObject->SetFlip(mCurFlip);
	}

	UpdateOffsetText();
	UpdateFrameText();
}

void CHairEditorWidget::OnClickFrontBackToggle()
{
	mIsMoveFrontHair = !mIsMoveFrontHair;

	mDisplayObject->SetOffset(mIsMoveFrontHair, GetHairOffset());

	UpdateOffsetText();
	UpdateTogleText();
}

void CHairEditorWidget::OnClickNextFrame()
{
	mCurFrameIndex = (mCurFrameIndex + 1) % mDisplayTextureFilePaths.size();
	mCurFrontOffset = mCurBackOffset = FVector2(0.f, 0.f);

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();
	WorldAssetMgr->LoadTexture("T_HairEditTexture_" + std::to_string(mCurFrameIndex), mDisplayTextureFilePaths[mCurFrameIndex].c_str());
	mCurTexture = WorldAssetMgr->FindTexture("T_HairEditTexture_" + std::to_string(mCurFrameIndex)).lock();

	mDisplayObject->SetTexture(mCurTexture, 0);

	if (mCurMetaData != nullptr)
	{
		const FHairFrameData* FrameMetaData = mCurMetaData->GetFrameMetaData<FHairFrameData>(mCurFrameIndex);
		if (FrameMetaData != nullptr)
		{
			mCurFrontOffset = FrameMetaData->mFrontHairOffset;
			mCurBackOffset = FrameMetaData->mBackHairOffset;
			mCurFlip = FrameMetaData->mIsFlip;
		}
	}

	mDisplayObject->SetOffset(true, mCurFrontOffset);
	mDisplayObject->SetOffset(false, mCurBackOffset);
	mDisplayObject->SetFlip(mCurFlip);

	UpdateOffsetText();
	UpdateFrameText();
	UpdateFlipText();
}

void CHairEditorWidget::OnClickPreFrame()
{
	mCurFrameIndex = (mCurFrameIndex + mDisplayTextureFilePaths.size() - 1) % mDisplayTextureFilePaths.size();
	mCurFrontOffset = mCurBackOffset = FVector2(0.f, 0.f);

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();
	WorldAssetMgr->LoadTexture("T_HairEditTexture_" + std::to_string(mCurFrameIndex), mDisplayTextureFilePaths[mCurFrameIndex].c_str());
	mCurTexture = WorldAssetMgr->FindTexture("T_HairEditTexture_" + std::to_string(mCurFrameIndex)).lock();

	mDisplayObject->SetTexture(mCurTexture, 0);

	if (mCurMetaData != nullptr)
	{
		const FHairFrameData* FrameMetaData = mCurMetaData->GetFrameMetaData<FHairFrameData>(mCurFrameIndex);
		if (FrameMetaData != nullptr)
		{
			mCurFrontOffset = FrameMetaData->mFrontHairOffset;
			mCurBackOffset = FrameMetaData->mBackHairOffset;
			mCurFlip = FrameMetaData->mIsFlip;
		}
	}

	mDisplayObject->SetOffset(true, mCurFrontOffset);
	mDisplayObject->SetOffset(false, mCurBackOffset);
	mDisplayObject->SetFlip(mCurFlip);

	UpdateOffsetText();
	UpdateFrameText();
	UpdateFlipText();
}

void CHairEditorWidget::OnClickMoveOffsetUp()
{
	FVector2& Offset = GetHairOffset();

	++Offset.y;
	mDisplayObject->SetOffset(mIsMoveFrontHair, Offset);

	UpdateOffsetText();
}

void CHairEditorWidget::OnClickMoveOffsetDown()
{
	FVector2& Offset = GetHairOffset();

	--Offset.y;
	mDisplayObject->SetOffset(mIsMoveFrontHair, Offset);

	UpdateOffsetText();
}

void CHairEditorWidget::OnClickMoveOffsetLeft()
{
	FVector2& Offset = GetHairOffset();

	--Offset.x;
	mDisplayObject->SetOffset(mIsMoveFrontHair, Offset);

	UpdateOffsetText();
}

void CHairEditorWidget::OnClickMoveOffsetRight()
{
	FVector2& Offset = GetHairOffset();

	++Offset.x;
	mDisplayObject->SetOffset(mIsMoveFrontHair, Offset);

	UpdateOffsetText();
}

void CHairEditorWidget::OnClickFlip()
{
	mCurFlip = !mCurFlip;

	mDisplayObject->SetFlip(mCurFlip);

	UpdateFlipText();
}

void CHairEditorWidget::OnClickPreCopy()
{
	if (mCurMetaData == nullptr)
	{
		return;
	}

	int32 PreFrameIndex = (mCurFrameIndex + mDisplayTextureFilePaths.size() - 1) % mDisplayTextureFilePaths.size();
	const FHairFrameData* FrameMetaData = mCurMetaData->GetFrameMetaData<FHairFrameData>(PreFrameIndex);
	if (FrameMetaData != nullptr)
	{
		mCurFrontOffset = FrameMetaData->mFrontHairOffset;
		mCurBackOffset = FrameMetaData->mBackHairOffset;
		mCurFlip = FrameMetaData->mIsFlip;
	}

	mDisplayObject->SetOffset(true, mCurFrontOffset);
	mDisplayObject->SetOffset(false, mCurBackOffset);
	mDisplayObject->SetFlip(mCurFlip);

	UpdateOffsetText();
	UpdateFrameText();
	UpdateFlipText();
}

void CHairEditorWidget::CreateEditObjects()
{
	if (mDisplayObject != nullptr)
	{
		mDisplayObject->Destroy();
	}
	std::shared_ptr<CWorld> World = mWorld.lock();
	mDisplayObject = World->CreateGameObject<CHairEditorObject>("DisPlayObject").lock();

	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();
	WorldAssetMgr->LoadTexture("T_HairEditTexture_" + std::to_string(mCurFrameIndex), mDisplayTextureFilePaths[mCurFrameIndex].c_str());
	mCurTexture = WorldAssetMgr->FindTexture("T_HairEditTexture_" + std::to_string(mCurFrameIndex)).lock();

	mDisplayObject->SetTexture(mCurTexture, 0);
	mDisplayObject->SetOffset(true, mCurFrontOffset);
	mDisplayObject->SetOffset(false, mCurBackOffset);
	mDisplayObject->SetFlip(mCurFlip);
}

void CHairEditorWidget::DestroyEditObjects()
{
	if (mDisplayObject != nullptr)
	{
		mDisplayObject->Destroy();
	}
	mDisplayObject = nullptr;
}

FVector2& CHairEditorWidget::GetHairOffset()
{
	if (mIsMoveFrontHair == true)
	{
		return mCurFrontOffset;
	}
	else
	{
		return mCurBackOffset;
	}
}

void CHairEditorWidget::UpdateTogleText()
{
	mFlipButton->SetEnable((mIsMoveFrontHair == true) ? true : false);
	mToggleText->SetText((mIsMoveFrontHair == true) ? TEXT("FH") : TEXT("BH"));
	mToggleText->SetTextColor((mIsMoveFrontHair == true) ? FVector4::Red : FVector4::Blue);
}

void CHairEditorWidget::UpdateOffsetText()
{
	const FVector2& Offset = GetHairOffset();
	mOffsetText->SetText(
		(TEXT('(') + std::to_wstring(static_cast<int32>(Offset.x)) + TEXT(',') + std::to_wstring(static_cast<int32>(Offset.y)) + TEXT(')')).c_str()
	);
}

void CHairEditorWidget::UpdateFrameText()
{
	mFrameText->SetText(
		(TEXT("Frame ") + std::to_wstring(mCurFrameIndex)).c_str()
	);
}

void CHairEditorWidget::UpdateFlipText()
{
	mFlipText->SetText((mCurFlip == true) ? TEXT("B") : TEXT("F"));
	mFlipText->SetTextColor((mCurFlip == true) ? FVector4::Blue : FVector4::Red);
}
