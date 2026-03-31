#include "Pch.h"
#include "TileEditorWidget.h"

#include "World/World.h"
#include "World/WorldAssetManager.h"

#include "UI/Editor/TileLayerEditorWidget.h"
#include "Editor/TileEditorObject.h"

#include "UI/Image.h"
#include "UI/Button.h"
#include "UI/TextBlock.h"

#include "Asset/Texture/Texture.h"

#include "Device.h"

#include "Setting/GlobalSetting.h"

#include "Component/MeshComponent.h"

bool CTileEditorWidget::Init()
{
	CWidgetContainer::Init();

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	mDisplayObject = World->CreateGameObject<CTileEditorObject>("DisplayObject").lock();
	mTestObject = World->CreateGameObject<CGameObject>("TestObject").lock();
	{
		std::shared_ptr<CMeshComponent> MeshComponent = mTestObject->CreateComponent<CMeshComponent>("MainMesh").lock();
		if (MeshComponent != nullptr)
		{
			MeshComponent->SetShader("DefaultTexture2D");
			MeshComponent->SetMesh("RectTex");
			MeshComponent->SetWorldScale(100.f, 100.f);
			MeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
			MeshComponent->SetBlendState(0, "AlphaBlend");
			MeshComponent->SetPivot(0.5f, 0.f);
			MeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture("T_PlayerIdle"));
		}
	}
	mTestObject->SetWorldPos(-25.f, 0.f);

	mLayerContainer = CreateWidget<CWidgetContainer>("LayerContainer", 0).lock();

	FResolution	RS = CDevice::GetInst()->GetResolution();

	FVector3 StartPos = FVector3::Zero;
	InitFirstLineButtons(RS, StartPos);

	StartPos.y += RS.Height * (mAllButtonSizeRateY + mPaddingRateY);
	InitSecondLineButtons(RS, StartPos);

	StartPos.y += RS.Height * (mAllButtonSizeRateY + mPaddingRateY);
	InitThirdLineButtons(RS, StartPos);

	StartPos.y += RS.Height * (mAllButtonSizeRateY + mPaddingRateY);
	InitLastLineButtons(RS, StartPos);

	StartPos.y += RS.Height * (mAllButtonSizeRateY);
	InitBackgorundImage(RS, StartPos);

	mLayerWidgetStartOffset = { RS.Width * (mSizeRateX - mLayerWidgetSizeRate.x) * 0.5f , StartPos.y + RS.Height * mPaddingRateY * 2.f };

	UpdateSelectionLockText();
	UpdateLayerText();
	UpdateTileIndexText();
	UpdateFrameText();
	UpdateRotZText();
	UpdateRenderEnableText();

	UpdateLayerWidgetLayouts();

	return true;
}

void CTileEditorWidget::InitFirstLineButtons(const FResolution& RS, const FVector3& StartPos)
{
	const float PaddingSizeX = RS.Width * mPaddingRateX;
	const FVector2 FontBoxSize = { (RS.Width * mSizeRateX - PaddingSizeX * 2.f) / 3.f, RS.Height * mTextSizeRateY };
	const float FontSize = std::min<float>(FontBoxSize.x, FontBoxSize.y) * 0.5f;

	mLayerText = CreateWidget<CTextBlock>("LayerText", 0).lock();
	{
		mLayerText->SetPivot(0.f, 0.f);
		mLayerText->SetPos(StartPos);
		mLayerText->SetSize(FontBoxSize);

		mLayerText->SetTextColor(FVector4::White);

		mLayerText->SetFont("EngineLight");
		mLayerText->SetFontSize(FontSize);

		mLayerText->SetAlignH(ETextAlignH::Left);
		mLayerText->SetAlignV(ETextAlignV::Middle);
	}

	mTileIndexText = CreateWidget<CTextBlock>("TileIndexText", 0).lock();
	{
		mTileIndexText->SetPivot(0.f, 0.f);
		mTileIndexText->SetPos(StartPos.x + FontBoxSize.x + PaddingSizeX, StartPos.y);
		mTileIndexText->SetSize(FontBoxSize);

		mTileIndexText->SetTextColor(FVector4::White);

		mTileIndexText->SetFont("EngineLight");
		mTileIndexText->SetFontSize(FontSize);

		mTileIndexText->SetAlignH(ETextAlignH::Left);
		mTileIndexText->SetAlignV(ETextAlignV::Middle);
	}

	mSelectionLockButton = CreateWidget<CButton>("SelectionLockButton", 0).lock();
	{
		mSelectionLockButton->SetPivot(0.f, 0.f);
		mSelectionLockButton->SetPos((StartPos.x + FontBoxSize.x + PaddingSizeX) * 2.f, StartPos.y);
		mSelectionLockButton->SetSize(FontBoxSize);

		mSelectionLockButton->SetWidgetColor(FVector4::Black);

		mSelectionLockButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickSelectionLock);

		std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("SelectionLockText", mWorld);
		mSelectionLockButton->SetChild(Text);
		{
			mSelectionLockText = Text;

			Text->SetPivot(0.f, 0.f);
			Text->SetSize(mSelectionLockButton->GetSize());
			Text->SetPos(0.f, 0.f);

			Text->SetFont("EngineLight");
			Text->SetFontSize(FontSize);

			Text->SetAlignH(ETextAlignH::Center);
			Text->SetAlignV(ETextAlignV::Middle);
		}
	}
}

void CTileEditorWidget::InitSecondLineButtons(const FResolution& RS, const FVector3& StartPos)
{
	const float PaddingSizeX = RS.Width * mPaddingRateX;
	const FVector2 ButtonSize = { (RS.Width * mSizeRateX - PaddingSizeX * 2.f) / 3.f, RS.Height * mAllButtonSizeRateY };
	const float FontSize = std::min<float>(ButtonSize.x, ButtonSize.y) * 0.5f;

	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("FirstContainer", 0).lock();
	Container->SetPivot(0.f, 0.f);
	Container->SetPos(StartPos);
	Container->SetSize(ButtonSize.x * 3 + PaddingSizeX * 2, ButtonSize.y);
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

			mSaveButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickSave);

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

			mLoadButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickLoad);

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
		mAddLayerButton = Container->CreateWidget<CButton>("AddLayerButton", 0).lock();
		{
			mAddLayerButton->SetPivot(0.f, 0.f);
			mAddLayerButton->SetPos((ButtonSize.x + PaddingSizeX) * 2.f, 0.f);
			mAddLayerButton->SetSize(ButtonSize);

			mAddLayerButton->SetWidgetColor(FVector4::Black);

			mAddLayerButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickAddLayer);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("AddLayerText", mWorld);
			mAddLayerButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mAddLayerButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("Add"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CTileEditorWidget::InitThirdLineButtons(const FResolution& RS, const FVector3& StartPos)
{
	const float PaddingSizeX = RS.Width * mPaddingRateX;
	const FVector2 ButtonSize = { (RS.Width * mSizeRateX - PaddingSizeX * 2.f) / 3.f, RS.Height * mAllButtonSizeRateY };
	const float FontSize = std::min<float>(ButtonSize.x, ButtonSize.y) * 0.5f;
	const float FontHalfSize = std::min<float>(ButtonSize.x * 0.5f, ButtonSize.y);

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
		mFrameText = Container->CreateWidget<CTextBlock>("FrameText", 0).lock();
		{
			mFrameText->SetPivot(0.f, 0.f);
			mFrameText->SetPos(0.f, 0.f);
			mFrameText->SetSize(ButtonSize);

			mFrameText->SetTextColor(FVector4::White);

			mFrameText->SetFont("EngineLight");
			mFrameText->SetFontSize(FontSize);

			mFrameText->SetAlignH(ETextAlignH::Center);
			mFrameText->SetAlignV(ETextAlignV::Middle);
		}
		mTileFramePreButton = Container->CreateWidget<CButton>("TileFramePreButton", 0).lock();
		{
			mTileFramePreButton->SetPivot(0.f, 0.f);
			mTileFramePreButton->SetPos((ButtonSize.x + PaddingSizeX) * 1.f, 0.f);
			mTileFramePreButton->SetSize(ButtonSize.x * 0.5f, ButtonSize.y);

			mTileFramePreButton->SetWidgetColor(FVector4::Black);

			mTileFramePreButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickPreTileFrame);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("TileFramePreText", mWorld);
			mTileFramePreButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mTileFramePreButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("◀"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontHalfSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mTileFrameNextButton = Container->CreateWidget<CButton>("TileFrameNextButton", 0).lock();
		{
			mTileFrameNextButton->SetPivot(0.f, 0.f);
			mTileFrameNextButton->SetPos((ButtonSize.x + PaddingSizeX) * 1.f + ButtonSize.x * 0.5f, 0.f);
			mTileFrameNextButton->SetSize(ButtonSize.x * 0.5f, ButtonSize.y);

			mTileFrameNextButton->SetWidgetColor(FVector4::Black);

			mTileFrameNextButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickNextTileFrame);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("TileFrameNextText", mWorld);
			mTileFrameNextButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mTileFrameNextButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("▶"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontHalfSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mRenderEnableButton = Container->CreateWidget<CButton>("RenderEnableButton", 0).lock();
		{
			mRenderEnableButton->SetPivot(0.f, 0.f);
			mRenderEnableButton->SetPos((ButtonSize.x + PaddingSizeX) * 2.f, 0.f);
			mRenderEnableButton->SetSize(ButtonSize);

			mRenderEnableButton->SetWidgetColor(FVector4::Black);

			mRenderEnableButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickRenderTile);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("RenderEnableText", mWorld);
			mRenderEnableButton->SetChild(Text);
			{
				mRenderEnableText = Text;

				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mRenderEnableButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CTileEditorWidget::InitLastLineButtons(const FResolution& RS, const FVector3& StartPos)
{
	const float PaddingSizeX = RS.Width * mPaddingRateX;
	const FVector2 ButtonSize = { (RS.Width * mSizeRateX - PaddingSizeX * 2.f) / 3.f, RS.Height * mAllButtonSizeRateY };
	const float FontSize = std::min<float>(ButtonSize.x, ButtonSize.y) * 0.5f;
	const float FontHalfSize = std::min<float>(ButtonSize.x * 0.5f, ButtonSize.y);

	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("ThirdContainer", 0).lock();
	Container->SetPivot(0.f, 0.f);
	Container->SetPos(StartPos);
	Container->SetSize(ButtonSize.x * 3 + PaddingSizeX * 2, ButtonSize.y);
	{
		std::shared_ptr<CImage> BackImage = Container->CreateWidget<CImage>("ThirdContainerBackImage", -1).lock();
		{
			BackImage->SetPivot(0.f, 0.f);
			BackImage->SetPos(0.f, 0.f);
			BackImage->SetSize(Container->GetSize());

			BackImage->SetWidgetColor(FVector4(0.1f, 0.1f, 0.1f, 1.f));
		}
		mRotZText = Container->CreateWidget<CTextBlock>("RotZText", 0).lock();
		{
			mRotZText->SetPivot(0.f, 0.f);
			mRotZText->SetPos(0.f, 0.f);
			mRotZText->SetSize(ButtonSize);

			mRotZText->SetTextColor(FVector4::White);

			mRotZText->SetFont("EngineLight");
			mRotZText->SetFontSize(FontSize);

			mRotZText->SetAlignH(ETextAlignH::Center);
			mRotZText->SetAlignV(ETextAlignV::Middle);
		}
		mTileRotPreButton = Container->CreateWidget<CButton>("TileRotPreButton", 0).lock();
		{
			mTileRotPreButton->SetPivot(0.f, 0.f);
			mTileRotPreButton->SetPos((ButtonSize.x + PaddingSizeX) * 1.f, 0.f);
			mTileRotPreButton->SetSize(ButtonSize.x * 0.5f, ButtonSize.y);

			mTileRotPreButton->SetWidgetColor(FVector4::Black);

			mTileRotPreButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickPreTileRotZ);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("TileRotPreText", mWorld);
			mTileRotPreButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mTileRotPreButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("◀"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontHalfSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mTileRotNextButton = Container->CreateWidget<CButton>("TileRotNextButton", 0).lock();
		{
			mTileRotNextButton->SetPivot(0.f, 0.f);
			mTileRotNextButton->SetPos((ButtonSize.x + PaddingSizeX) * 1.f + ButtonSize.x * 0.5f, 0.f);
			mTileRotNextButton->SetSize(ButtonSize.x * 0.5f, ButtonSize.y);

			mTileRotNextButton->SetWidgetColor(FVector4::Black);

			mTileRotNextButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickNextTileRotZ);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("TileRotNextText", mWorld);
			mTileRotNextButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mTileRotNextButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("▶"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontHalfSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mAllRenderFlipButton = Container->CreateWidget<CButton>("AllRenderFlipButton", 0).lock();
		{
			mAllRenderFlipButton->SetPivot(0.f, 0.f);
			mAllRenderFlipButton->SetPos((ButtonSize.x + PaddingSizeX) * 2.f, 0.f);
			mAllRenderFlipButton->SetSize(ButtonSize);

			mAllRenderFlipButton->SetWidgetColor(FVector4::Black);

			mAllRenderFlipButton->SetEventCallback(EButtonEventState::Click, this, &CTileEditorWidget::OnClickAllFlipTile);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("AllRenderFlipText", mWorld);
			mAllRenderFlipButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mAllRenderFlipButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("FlipAll"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CTileEditorWidget::InitBackgorundImage(const FResolution& RS, const FVector3& StartPos)
{
	const FVector2 ImageSize = { RS.Width * mSizeRateX, RS.Height * (mTextSizeRateY + mAllButtonSizeRateY * 3 + mPaddingRateY * 3) };

	std::shared_ptr<CImage> BackImage = CreateWidget<CImage>("BackgroundImage", -1).lock();
	{
		BackImage->SetPivot(0.f, 0.f);
		BackImage->SetPos(0.f, 0.f);
		BackImage->SetSize(ImageSize);

		BackImage->SetWidgetColor(FVector4(0.2f, 0.2f, 0.2f, 1.f));
	}
}

bool CTileEditorWidget::IsValidCopyData() const
{
	if (IsValidLayer() == false)
	{
		return false;
	}

	int32 FrameCount = mDisplayObject->GetTileFrameCount(mCurLayer);
	return mCopyFrame >= 0 && mCopyFrame < FrameCount && mCopyRotZ >= 0.f;
}

void CTileEditorWidget::OnClickSelectionLock()
{
	mIsSelectionLock = !mIsSelectionLock;

	UpdateSelectionLockText();
}

void CTileEditorWidget::OnClickSave()
{
	if (mDisplayObject == nullptr)
	{
		return;
	}

	if (IsValidTileIndex() == true)
	{
		mDisplayObject->SetTileHighlight(mCurLayer, mCurTileIndexX, mCurTileIndexY, FVector4(0.f, 0.f, 0.f, 0.f));
	}

	for (auto& LayerWidget : mLayerEditorWidgets)
	{
		mDisplayObject->SaveMap(LayerWidget->GetLayerName());
	}

	if (IsValidTileIndex() == true)
	{
		mDisplayObject->SetTileHighlight(mCurLayer, mCurTileIndexX, mCurTileIndexY, mHightLightColor);
	}
}

void CTileEditorWidget::OnClickLoad()
{
	if (mDisplayObject == nullptr)
	{
		return;
	}

	FResolution	RS = CDevice::GetInst()->GetResolution();
	std::shared_ptr<CTileLayerEditorWidget> NewLayer = mLayerContainer->CreateWidget<CTileLayerEditorWidget>("TileLayerEditor" + std::to_string(mMaxLayerNameIndex), 0).lock();
	{
		NewLayer->SetParentEditor(std::static_pointer_cast<CTileEditorWidget>(mSelf.lock()));
		NewLayer->SetPivot(0.f, 0.f);
		NewLayer->SetPos(mLayerWidgetStartOffset);
		NewLayer->SetSize(mLayerWidgetSizeRate.x * RS.Width, mLayerWidgetSizeRate.y * RS.Height);
		NewLayer->SetLayerIndex(mMaxLayerNameIndex);
	}
	mLayerEditorWidgets.push_back(NewLayer);

	const std::string NewLayerName = NewLayer->GetLayerName();
	mDisplayObject->CeateNewLayer(NewLayerName, "Tile0");
	++mMaxLayerNameIndex;

	mDisplayObject->LoadMap(NewLayerName);
	mDisplayObject->BeginLayer(NewLayerName);

	UpdateLayerWidgetLayouts();
	NewLayer->UpdateLayerNameText();
	NewLayer->UpdateRenderLayerNameText();
	NewLayer->UpdateEnableText();
}

void CTileEditorWidget::OnClickAddLayer()
{
	if (mDisplayObject == nullptr)
	{
		return;
	}

	FResolution	RS = CDevice::GetInst()->GetResolution();
	std::shared_ptr<CTileLayerEditorWidget> NewLayer = mLayerContainer->CreateWidget<CTileLayerEditorWidget>("TileLayerEditor" + std::to_string(mMaxLayerNameIndex), 0).lock();
	{
		NewLayer->SetParentEditor(std::static_pointer_cast<CTileEditorWidget>(mSelf.lock()));
		NewLayer->SetPivot(0.f, 0.f);
		NewLayer->SetPos(mLayerWidgetStartOffset);
		NewLayer->SetSize(mLayerWidgetSizeRate.x * RS.Width, mLayerWidgetSizeRate.y * RS.Height);
		NewLayer->SetLayerIndex(mMaxLayerNameIndex);
	}
	mLayerEditorWidgets.push_back(NewLayer);

	const std::string NewLayerName = NewLayer->GetLayerName();
	mDisplayObject->CeateNewLayer(NewLayerName, "Tile0");
	++mMaxLayerNameIndex;

	for (int32 i = 0; i < ETileTextureType::End; ++i)
	{
		mDisplayObject->SetLayerTexture(NewLayerName, static_cast<ETileTextureType::Type>(i));
	}
	mDisplayObject->SetLayerTileFrameSize(NewLayerName);
	mDisplayObject->SetLayerTileSize(NewLayerName);
	mDisplayObject->BeginLayer(NewLayerName);

	UpdateLayerWidgetLayouts();
	NewLayer->UpdateLayerNameText();
	NewLayer->UpdateRenderLayerNameText();
	NewLayer->UpdateEnableText();
}

void CTileEditorWidget::OnClickNextTileFrame()
{
	if (mDisplayObject == nullptr || IsValidTileIndex() == false)
	{
		return;
	}

	mDisplayObject->AddTileFrame(mCurLayer, mCurTileIndexX, mCurTileIndexY, 1);
	UpdateFrameText();
	UpdateRenderEnableText();

	/*for (int y = 1; y < 25; ++y)
	{
		for (int x = 0; x < 60; ++x)
		{
			bool IsRender = mDisplayObject->GetTileRender(mCurLayer, x, y);
			int Index = mDisplayObject->GetTileFrame(mCurLayer, x, y);
			mDisplayObject->SetTileFrame(mCurLayer, x, y - 1, Index);
			mDisplayObject->SetTileRender(mCurLayer, x, y - 1, IsRender);
		}
	}*/
}

void CTileEditorWidget::OnClickPreTileFrame()
{
	if (mDisplayObject == nullptr || IsValidTileIndex() == false)
	{
		return;
	}

	mDisplayObject->AddTileFrame(mCurLayer, mCurTileIndexX, mCurTileIndexY, -1);
	UpdateFrameText();
	UpdateRenderEnableText();
}

void CTileEditorWidget::OnClickRenderTile()
{
	if (mDisplayObject == nullptr || IsValidTileIndex() == false)
	{
		return;
	}

	mDisplayObject->FlipTileRender(mCurLayer, mCurTileIndexX, mCurTileIndexY);
	UpdateRenderEnableText();
}

void CTileEditorWidget::OnClickNextTileRotZ()
{
	if (mDisplayObject == nullptr || IsValidTileIndex() == false)
	{
		return;
	}

	mDisplayObject->AddTileRotZ(mCurLayer, mCurTileIndexX, mCurTileIndexY, -90.f);
	UpdateRotZText();
}

void CTileEditorWidget::OnClickPreTileRotZ()
{
	if (mDisplayObject == nullptr || IsValidTileIndex() == false)
	{
		return;
	}

	mDisplayObject->AddTileRotZ(mCurLayer, mCurTileIndexX, mCurTileIndexY, 90.f);
	UpdateRotZText();
}

void CTileEditorWidget::OnClickAllFlipTile()
{
	if (mDisplayObject == nullptr || IsValidTileIndex() == false)
	{
		return;
	}

	mDisplayObject->FlipAllTileRender(mCurLayer);
	UpdateRenderEnableText();
}

void CTileEditorWidget::OnClickResizeLayer(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget)
{
	if (mDisplayObject == nullptr)
	{
		return;
	}

	mDisplayObject->ResizeLayer(LayerName);

	if (mCurLayer == LayerWidget->GetLayerName())
	{
		ResetTileSelection();
	}
}

void CTileEditorWidget::OnClickEnableLayer(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget)
{
	if (mDisplayObject == nullptr)
	{
		return;
	}
	mDisplayObject->FlipLayerEnable(LayerName);
	LayerWidget->SetIsEnable(!LayerWidget->GetIsEnable());

	if (mCurLayer == LayerWidget->GetLayerName())
	{
		ResetTileSelection();
	}
	else if (LayerWidget->GetIsEnable() == true)
	{
		for (auto& LayerEditorWidget : mLayerEditorWidgets)
		{
			if (LayerEditorWidget->GetLayerName() == mCurLayer)
			{
				break;
			}
			if (LayerEditorWidget == LayerWidget)
			{
				ResetTileSelection();
				break;
			}
		}
	}
	
	LayerWidget->UpdateEnableText();
}

void CTileEditorWidget::OnClickRenderLayerUp(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget)
{
	if (mDisplayObject == nullptr)
	{
		return;
	}

	int32 NewRenderIndex = std::min<int32>(LayerWidget->GetRenderLayerIndex() + 1, MAX_TILE_RENDER_LAYER - 1);
	LayerWidget->SetRenderLayerIndex(NewRenderIndex);
	mDisplayObject->SwapRenderLayer(LayerName, LayerWidget->GetRenderLayerName());

	auto SwapIter = mLayerEditorWidgets.begin();
	for (; SwapIter != mLayerEditorWidgets.end(); ++SwapIter)
	{
		if ((*SwapIter)->GetRenderLayerIndex() <= NewRenderIndex)
		{
			break;
		}
	}

	if (SwapIter != mLayerEditorWidgets.end() && *SwapIter != LayerWidget)
	{
		if (mCurLayer == LayerWidget->GetLayerName() || mCurLayer == (*SwapIter)->GetLayerName())
		{
			ResetTileSelection();
		}

		auto Iter = SwapIter;
		for (++Iter; Iter != mLayerEditorWidgets.end(); ++Iter)
		{
			if (*Iter == LayerWidget)
			{
				std::swap(*Iter, *SwapIter);
				break;
			}
		}
		
		UpdateLayerWidgetLayouts();
	}
	LayerWidget->UpdateRenderLayerNameText();
}

void CTileEditorWidget::OnClickRenderLayerDown(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget)
{
	if (mDisplayObject == nullptr)
	{
		return;
	}

	int32 NewRenderIndex = std::max<int32>(LayerWidget->GetRenderLayerIndex() - 1, 0);
	LayerWidget->SetRenderLayerIndex(NewRenderIndex);
	mDisplayObject->SwapRenderLayer(LayerName, LayerWidget->GetRenderLayerName());

	auto SwapIter = mLayerEditorWidgets.rbegin();
	for (; SwapIter != mLayerEditorWidgets.rend(); ++SwapIter)
	{
		if ((*SwapIter)->GetRenderLayerIndex() >= NewRenderIndex)
		{
			break;
		}
	}

	if (SwapIter != mLayerEditorWidgets.rend() && *SwapIter != LayerWidget)
	{
		if (mCurLayer == LayerWidget->GetLayerName() || mCurLayer == (*SwapIter)->GetLayerName())
		{
			ResetTileSelection();
		}

		auto Iter = SwapIter;
		for (++Iter; Iter != mLayerEditorWidgets.rend(); ++Iter)
		{
			if (*Iter == LayerWidget)
			{
				std::swap(*Iter, *SwapIter);
				break;
			}
		}
		
		UpdateLayerWidgetLayouts();
	}
	LayerWidget->UpdateRenderLayerNameText();
}

void CTileEditorWidget::OnClickRemoveLayer(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget)
{
	if (mDisplayObject == nullptr)
	{
		return;
	}
	mDisplayObject->RemoveLayer(LayerName);

	for (auto Iter = mLayerEditorWidgets.begin(); Iter != mLayerEditorWidgets.end(); ++Iter)
	{
		if (*Iter == LayerWidget)
		{
			LayerWidget->Destroy();
			mLayerEditorWidgets.erase(Iter);
			break;
		}
	}

	if (mCurLayer == LayerWidget->GetLayerName())
	{
		ResetTileSelection();
	}
	UpdateLayerWidgetLayouts();
}

void CTileEditorWidget::UpdateSelectionLockText()
{
	mSelectionLockText->SetText(
		(mIsSelectionLock == true) ? TEXT("LOCK") : TEXT("UNLOCK")
	);
	mSelectionLockText->SetTextColor(
		(mIsSelectionLock == true) ? FVector4::Red : FVector4::Green
	);
}

void CTileEditorWidget::UpdateLayerText()
{
	if (IsValidLayer() == true)
	{
		std::wstring CurLayer = StringUtils::ConvertUtf8ToWString(mCurLayer);
		mLayerText->SetText(
			(TEXT("Layer ") + CurLayer).c_str()
		);
	}
	else
	{
		mLayerText->SetText(
			TEXT("Layer -")
		);
	}
}

void CTileEditorWidget::UpdateTileIndexText()
{
	if (IsValidTileIndex() == true)
	{
		mTileIndexText->SetText(
			(TEXT("Index ") + std::to_wstring(static_cast<int32>(mCurTileIndexX)) + TEXT(",") + std::to_wstring(static_cast<int32>(mCurTileIndexY))).c_str()
		);
	}
	else
	{
		mTileIndexText->SetText(
			TEXT("Index -,-")
		);
	}
}

void CTileEditorWidget::UpdateFrameText()
{
	if (mDisplayObject != nullptr && IsValidTileIndex() == true)
	{
		int32 Frame = mDisplayObject->GetTileFrame(mCurLayer, mCurTileIndexX, mCurTileIndexY);
		if (Frame >= 0)
		{
			mFrameText->SetText(
				(TEXT("Frame ") + std::to_wstring(Frame)).c_str()
			);
			return;
		}
	}
	else
	{
		mFrameText->SetText(
			TEXT("Frame -")
		);
	}
}

void CTileEditorWidget::UpdateRotZText()
{
	if (mDisplayObject != nullptr && IsValidTileIndex() == true)
	{
		float RotZ = mDisplayObject->GetTileRotZ(mCurLayer, mCurTileIndexX, mCurTileIndexY);
		if (RotZ >= 0.f)
		{
			mRotZText->SetText(
				(TEXT("Rot ") + std::to_wstring(static_cast<int32>(RotZ))).c_str()
			);
			return;
		}
	}

	mRotZText->SetText(
		TEXT("Rot -")
	);
}

void CTileEditorWidget::UpdateRenderEnableText()
{
	if (mDisplayObject != nullptr && IsValidTileIndex() == true)
	{
		bool Render = mDisplayObject->GetTileRender(mCurLayer, mCurTileIndexX, mCurTileIndexY);
		mRenderEnableText->SetText(
			(Render == true) ? TEXT("SHOW") : TEXT("HIDE")
		);
		mRenderEnableText->SetTextColor(
			(Render == true) ? FVector4::Green : FVector4::Red
		);
		return;
	}

	mRenderEnableText->SetText(
		TEXT("HIDE")
	);
	mRenderEnableText->SetTextColor(
		FVector4::Red
	);
}

void CTileEditorWidget::UpdateLayerWidgetLayouts()
{
	FResolution	RS = CDevice::GetInst()->GetResolution();
	const float WidgetOffsetY = RS.Height * (mLayerWidgetSizeRate.y + mPaddingRateY);

	int32 i = 0;
	for (auto& LayerWidget : mLayerEditorWidgets)
	{
		LayerWidget->SetPos(mLayerWidgetStartOffset + FVector2(0.f, i * WidgetOffsetY));
		++i;
	}
}

void CTileEditorWidget::ShowEditObjects()
{
	SetEnable(true);
	if (mDisplayObject != nullptr)
	{
		mDisplayObject->SetEnable(true);
	}
	if (mTestObject != nullptr)
	{
		mTestObject->SetEnable(true);
	}
}

void CTileEditorWidget::HideEditObjects()
{
	SetEnable(false);
	if (mDisplayObject != nullptr)
	{
		mDisplayObject->SetEnable(false);
	}
	if (mTestObject != nullptr)
	{
		mTestObject->SetEnable(false);
	}
}

void CTileEditorWidget::ResetTileSelection()
{
	if (IsValidTileIndex() == false)
	{
		return;
	}

	mDisplayObject->SetTileHighlight(mCurLayer, mCurTileIndexX, mCurTileIndexY, FVector4(0.f, 0.f, 0.f, 0.f));

	mCurLayer = "";
	mCurTileIndexX = mCurTileIndexY = -1;

	UpdateLayerText();
	UpdateTileIndexText();
	UpdateFrameText();
	UpdateRotZText();
	UpdateRenderEnableText();
}

void CTileEditorWidget::CheckTileUnderMouse()
{
	if (GetEnable() == false || mDisplayObject == nullptr || mIsSelectionLock == true)
	{
		return;
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CInput> Input = World->GetInput().lock();

	FVector2 MousePos = Input->GetMouseWorldPos();

	std::string LayerName;
	for (auto& LayerWidget : mLayerEditorWidgets)
	{
		if (LayerWidget->GetIsEnable() == true)
		{
			LayerName = LayerWidget->GetLayerName();
			break;
		}
	}
	if (LayerName.empty() == true)
	{
		return;
	}

	const int32 PreCurTileIndexX = mCurTileIndexX;
	const int32 PreCurTileIndexY = mCurTileIndexY;
	if (IsValidTileIndex() == true)
	{
		mDisplayObject->SetTileHighlight(mCurLayer, mCurTileIndexX, mCurTileIndexY, FVector4(0.f, 0.f, 0.f, 0.f));
	}

	mDisplayObject->GetTileIndex(LayerName, MousePos, OUT mCurTileIndexX, OUT mCurTileIndexY);
	if (mCurTileIndexX < 0)
	{
		mCurTileIndexX = PreCurTileIndexX;
		mCurTileIndexY = PreCurTileIndexY;
	}
	else
	{
		mCurLayer = LayerName;
	}

	if (IsValidTileIndex() == true)
	{
		mDisplayObject->SetTileHighlight(mCurLayer, mCurTileIndexX, mCurTileIndexY, mHightLightColor);
	}

	UpdateLayerText();
	UpdateTileIndexText();
	UpdateFrameText();
	UpdateRotZText();
	UpdateRenderEnableText();
}

void CTileEditorWidget::ResetCopyData()
{
	mCopyFrame = -1;
	mCopyRotZ = -1.f;
}

void CTileEditorWidget::CopyFrameAndRotZ()
{
	if (GetEnable() == false || mDisplayObject == nullptr || mIsSelectionLock == true)
	{
		return;
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CInput> Input = World->GetInput().lock();

	FVector2 MousePos = Input->GetMouseWorldPos();

	std::string LayerName;
	for (auto& LayerWidget : mLayerEditorWidgets)
	{
		if (LayerWidget->GetIsEnable() == true)
		{
			LayerName = LayerWidget->GetLayerName();
			break;
		}
	}
	if (LayerName.empty() == true)
	{
		return;
	}

	int32 CopyTileIndexX = -1;
	int32 CopyTileIndexY = -1;
	mDisplayObject->GetTileIndex(LayerName, MousePos, OUT CopyTileIndexX, OUT CopyTileIndexY);
	if (CopyTileIndexX < 0)
	{
		ResetCopyData();
		return;
	}
	else
	{
		mCopyFrame = mDisplayObject->GetTileFrame(LayerName, CopyTileIndexX, CopyTileIndexY);
		mCopyRotZ = mDisplayObject->GetTileRotZ(LayerName, CopyTileIndexX, CopyTileIndexY);
	}
}

void CTileEditorWidget::PasteFrameAndRotZ()
{
	if (IsValidCopyData() == false)
	{
		return;
	}

	if (GetEnable() == false || mDisplayObject == nullptr || mIsSelectionLock == true)
	{
		return;
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CInput> Input = World->GetInput().lock();

	FVector2 MousePos = Input->GetMouseWorldPos();

	std::string LayerName;
	for (auto& LayerWidget : mLayerEditorWidgets)
	{
		if (LayerWidget->GetIsEnable() == true)
		{
			LayerName = LayerWidget->GetLayerName();
			break;
		}
	}
	if (LayerName.empty() == true)
	{
		return;
	}

	int32 CopyTileIndexX = -1;
	int32 CopyTileIndexY = -1;
	mDisplayObject->GetTileIndex(LayerName, MousePos, OUT CopyTileIndexX, OUT CopyTileIndexY);
	if (CopyTileIndexX < 0)
	{
		return;
	}

	mDisplayObject->SetTileFrame(LayerName, CopyTileIndexX, CopyTileIndexY, mCopyFrame);
	mDisplayObject->SetTileRotZ(LayerName, CopyTileIndexX, CopyTileIndexY, mCopyRotZ);

	if (mCurTileIndexX == CopyTileIndexX && mCurTileIndexY == CopyTileIndexY)
	{
		UpdateFrameText();
		UpdateRotZText();
		UpdateRenderEnableText();
	}
}
