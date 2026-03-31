#include "Pch.h"
#include "EditorWidget.h"

#include "UI/Image.h"
#include "UI/Button.h"
#include "UI/TextBlock.h"

#include "Device.h"

#include "UI/Editor/HairEditorWidget.h"
#include "UI/Editor/TileEditorWidget.h"

bool CEditorWidget::Init()
{
	CWidgetContainer::Init();

	FResolution	RS = CDevice::GetInst()->GetResolution();

	FVector3 ButtonStartPos;
	ButtonStartPos.x = RS.Width * mAllButtonOffsetRate.x;
	ButtonStartPos.y = RS.Height * mAllButtonOffsetRate.y;

	InitModeText(RS, ButtonStartPos);
	InitModeButtons(RS, ButtonStartPos);

	FVector3 ModeWidgetStartPos;
	ModeWidgetStartPos.x = RS.Width * mAllModeWidgetOffsetRate.x;
	ModeWidgetStartPos.y = RS.Height * mAllModeWidgetOffsetRate.y;

	InitModeWidgets(RS, ModeWidgetStartPos);

	OnActivateHairEditMode();

	return true;
}

void CEditorWidget::InitModeText(const FResolution& RS, const FVector3& ButtonStartPos)
{
	mEditModeText = CreateWidget<CTextBlock>("ModeText", 0).lock();
	{
		const float FontSize = std::min<float>(RS.Width * mTextSizeRate.x, RS.Height * mTextSizeRate.y);

		mEditModeText->SetPivot(0.f, 0.f);
		mEditModeText->SetPos(ButtonStartPos.x, ButtonStartPos.y - RS.Height * mTextSizeRate.y);
		mEditModeText->SetSize(RS.Width * mAllButtonSizeRate.x * 2, RS.Height * mTextSizeRate.y);

		mEditModeText->SetText(TEXT("None"));
		mEditModeText->SetTextColor(FVector4::White);

		mEditModeText->SetFont("EngineDefault");
		mEditModeText->SetFontSize(FontSize);

		mEditModeText->EnableOutline(true);
		mEditModeText->SetOutlineTextColor(FVector4::Blue);
		mEditModeText->SetOutlineWidth(2.f);

		mEditModeText->SetAlignH(ETextAlignH::Center);
		mEditModeText->SetAlignV(ETextAlignV::Middle);
	}
}

void CEditorWidget::InitModeButtons(const FResolution& RS, const FVector3& ButtonStartPos)
{
	const FVector2 ButtonSize = { RS.Width * mAllButtonSizeRate.x, RS.Height * mAllButtonSizeRate.y };

	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("ModeContainer", 0).lock();
	Container->SetPivot(0.f, 0.f);
	Container->SetPos(ButtonStartPos);
	Container->SetSize(ButtonSize.x * 2, ButtonSize.y);
	Container->SetWidgetColor(FVector4(0.2f, 0.2f, 0.2f, 1.f));
	{
		std::shared_ptr<CImage> BackImage = Container->CreateWidget<CImage>("ModeContainerBackImage", -1).lock();
		{
			BackImage->SetPivot(0.f, 0.f);
			BackImage->SetPos(0.f, 0.f);
			BackImage->SetSize(Container->GetSize());

			BackImage->SetWidgetColor(FVector4(0.2f, 0.2f, 0.2f, 1.f));
		}
		mHairEditButton = Container->CreateWidget<CButton>("EditHairButton", 0).lock();
		{
			mHairEditButton->SetEnable(false);

			mHairEditButton->SetPivot(0.f, 0.f);
			mHairEditButton->SetPos(0.f, 0.f);
			mHairEditButton->SetSize(ButtonSize);

			mHairEditButton->SetWidgetColor(FVector4(0.2f, 0.2f, 0.8f, 1.f));

			mHairEditButton->SetEventCallback(EButtonEventState::Click, this, &CEditorWidget::OnActivateHairEditMode);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("EditHairText", mWorld);
			mHairEditButton->SetChild(Text);
			{
				const float FontSize = std::min<float>(mHairEditButton->GetSize().x, mHairEditButton->GetSize().y);

				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mHairEditButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("Hair"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineDefault");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mTileEditButton = Container->CreateWidget<CButton>("EditTileButton", 0).lock();
		{
			mTileEditButton->SetEnable(false);

			mTileEditButton->SetPivot(0.f, 0.f);
			mTileEditButton->SetPos(ButtonSize.x, 0.f);
			mTileEditButton->SetSize(ButtonSize);

			mTileEditButton->SetWidgetColor(FVector4(0.2f, 0.8f, 0.2f, 1.f));

			mTileEditButton->SetEventCallback(EButtonEventState::Click, this, &CEditorWidget::OnActivateTileEditMode);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("EditTileText", mWorld);
			mTileEditButton->SetChild(Text);
			{
				const float FontSize = std::min<float>(mTileEditButton->GetSize().x, mTileEditButton->GetSize().y);

				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mTileEditButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("Tile"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineDefault");
				Text->SetFontSize(FontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CEditorWidget::InitModeWidgets(const FResolution& RS, const FVector3& ModeWidgetStartPos)
{
	mHairEditWidget = CreateWidget<CHairEditorWidget>("HairEditWidget", 0).lock();
	{
		mHairEditWidget->SetEnable(false);

		mHairEditWidget->SetPos(ModeWidgetStartPos);
		mHairEditWidget->SetPivot(0.f, 0.f);
	}
	mTileEditWidget = CreateWidget<CTileEditorWidget>("TileEditWidget", 0).lock();
	{
		mTileEditWidget->SetEnable(false);

		mTileEditWidget->SetPos(ModeWidgetStartPos);
		mTileEditWidget->SetPivot(0.f, 0.f);
	}
}

void CEditorWidget::OnActivateHairEditMode()
{
	mMode = EEditorMode::Hair;
	mEditModeText->SetText(TEXT("Hair Mode"));
	mEditModeText->SetOutlineTextColor(mHairEditButton->GetWidgetColor());

	mHairEditButton->SetEnable(false);
	mTileEditButton->SetEnable(true);

	mTileEditWidget->SetEnable(false);
	mTileEditWidget->HideEditObjects();

	mHairEditWidget->SetEnable(true);
	mHairEditWidget->CreateEditObjects();
}

void CEditorWidget::OnActivateTileEditMode()
{
	mMode = EEditorMode::Tile;
	mEditModeText->SetText(TEXT("Tile Mode"));
	mEditModeText->SetOutlineTextColor(mTileEditButton->GetWidgetColor());

	mTileEditButton->SetEnable(false);
	mHairEditButton->SetEnable(true);

	mHairEditWidget->SetEnable(false);
	mHairEditWidget->DestroyEditObjects();

	mTileEditWidget->SetEnable(true);
	mTileEditWidget->ShowEditObjects();
}

