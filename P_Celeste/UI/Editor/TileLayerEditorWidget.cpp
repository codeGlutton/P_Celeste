#include "Pch.h"
#include "TileLayerEditorWidget.h"

#include "World/World.h"

#include "Editor/TileEditorObject.h"
#include "UI/Editor/TileEditorWidget.h"

#include "UI/Image.h"
#include "UI/Button.h"
#include "UI/TextBlock.h"

#include "Asset/Texture/Texture.h"

#include "Device.h"

bool CTileLayerEditorWidget::Init()
{
	CWidgetContainer::Init();

	FResolution	RS = CDevice::GetInst()->GetResolution();
	InitFirstLine(RS);

	UpdateLayerNameText();
	UpdateRenderLayerNameText();
	UpdateEnableText();

	return true;
}

void CTileLayerEditorWidget::InitFirstLine(const FResolution& RS)
{
	const float PaddingSizeX = RS.Width * mPaddingRateX;
	const FVector2 ButtonSize = {
		(RS.Width * (mSizeRate.x - mTextSizeRateX * 2.f) - PaddingSizeX * 5.f) / 5.f,
		RS.Height * mSizeRate.y 
	};
	const FVector2 TextSize = {
		RS.Width* mTextSizeRateX,
		RS.Height* mSizeRate.y
	};
	const float FontSize = std::min<float>(TextSize.x, TextSize.y) * 0.5f;
	const float BoxHalfFontSize = std::min<float>(ButtonSize.x, ButtonSize.y) * 0.5f;
	const float BoxFontSize = std::min<float>(ButtonSize.x, ButtonSize.y);

	std::shared_ptr<CWidgetContainer> Container = CreateWidget<CWidgetContainer>("FirstContainer", 0).lock();
	Container->SetPivot(0.f, 0.f);
	Container->SetPos(0.f, 0.f);
	Container->SetSize(mSizeRate.x * RS.Width, ButtonSize.y);
	{
		std::shared_ptr<CImage> BackImage = Container->CreateWidget<CImage>("FirstContainerBackImage", -1).lock();
		{
			BackImage->SetPivot(0.f, 0.f);
			BackImage->SetPos(0.f, 0.f);
			BackImage->SetSize(Container->GetSize());

			BackImage->SetWidgetColor(FVector4(0.2f, 0.2f, 0.2f, 1.f));
		}
		mLayerNameText = Container->CreateWidget<CTextBlock>("LayerNameText", 0).lock();
		{
			mLayerNameText->SetPivot(0.f, 0.f);
			mLayerNameText->SetPos(0.f, 0.f);
			mLayerNameText->SetSize(TextSize);

			mLayerNameText->SetTextColor(FVector4::White);

			mLayerNameText->SetFontSize(FontSize);

			mLayerNameText->SetAlignH(ETextAlignH::Left);
			mLayerNameText->SetAlignV(ETextAlignV::Middle);
		}
		mRenderLayerNameText = Container->CreateWidget<CTextBlock>("RenderLayerNameText", 0).lock();
		{
			mRenderLayerNameText->SetPivot(0.f, 0.f);
			mRenderLayerNameText->SetPos((TextSize.x + PaddingSizeX) * 1.f, 0.f);
			mRenderLayerNameText->SetSize(TextSize);

			mRenderLayerNameText->SetTextColor(FVector4::White);

			mRenderLayerNameText->SetFontSize(FontSize);

			mRenderLayerNameText->SetAlignH(ETextAlignH::Left);
			mRenderLayerNameText->SetAlignV(ETextAlignV::Middle);
		}
		mResizeButton = Container->CreateWidget<CButton>("ResizeButton", 0).lock();
		{
			mResizeButton->SetPivot(0.f, 0.f);
			mResizeButton->SetPos((TextSize.x + PaddingSizeX) * 2.f, 0.f);
			mResizeButton->SetSize(ButtonSize);

			mResizeButton->SetWidgetColor(FVector4::Black);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("ResizeText", mWorld);
			mResizeButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mResizeButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("Resize"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(BoxHalfFontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mEnableButton = Container->CreateWidget<CButton>("EnableButton", 0).lock();
		{
			mEnableButton->SetPivot(0.f, 0.f);
			mEnableButton->SetPos(
				(TextSize.x + PaddingSizeX) * 2.f + (ButtonSize.x + PaddingSizeX) * 1.f,
				0.f
			);
			mEnableButton->SetSize(ButtonSize);

			mEnableButton->SetWidgetColor(FVector4::Black);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("EnableText", mWorld);
			mEnableButton->SetChild(Text);
			{
				mEnableText = Text;

				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mEnableButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetFont("EngineLight");
				Text->SetFontSize(BoxHalfFontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mRenderLayerDownButton = Container->CreateWidget<CButton>("RenderLayerDownButton", 0).lock();
		{
			mRenderLayerDownButton->SetPivot(0.f, 0.f);
			mRenderLayerDownButton->SetPos(
				(TextSize.x + PaddingSizeX) * 2.f + (ButtonSize.x + PaddingSizeX) * 2.f,
				0.f
			);
			mRenderLayerDownButton->SetSize(ButtonSize);

			mRenderLayerDownButton->SetWidgetColor(FVector4::Black);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("RenderLayerDownText", mWorld);
			mRenderLayerDownButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mRenderLayerDownButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("◀"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(BoxFontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mRenderLayerUpButton = Container->CreateWidget<CButton>("RenderLayerUpButton", 0).lock();
		{
			mRenderLayerUpButton->SetPivot(0.f, 0.f);
			mRenderLayerUpButton->SetPos(
				(TextSize.x + PaddingSizeX) * 2.f + (ButtonSize.x + PaddingSizeX) * 2.f + ButtonSize.x,
				0.f
			);
			mRenderLayerUpButton->SetSize(ButtonSize);

			mRenderLayerUpButton->SetWidgetColor(FVector4::Black);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("RenderLayerUpText", mWorld);
			mRenderLayerUpButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mRenderLayerUpButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("▶"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(BoxFontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
		mRemoveLayerButton = Container->CreateWidget<CButton>("RemoveLayerButton", 0).lock();
		{
			mRemoveLayerButton->SetPivot(0.f, 0.f);
			mRemoveLayerButton->SetPos(
				(TextSize.x + PaddingSizeX) * 2.f + (ButtonSize.x + PaddingSizeX) * 3.f + ButtonSize.x,
				0.f
			);
			mRemoveLayerButton->SetSize(ButtonSize);

			mRemoveLayerButton->SetWidgetColor(FVector4::Black);

			std::shared_ptr<CTextBlock> Text = CreateStaticWidget<CTextBlock>("RemoveLayerText", mWorld);
			mRemoveLayerButton->SetChild(Text);
			{
				Text->SetPivot(0.f, 0.f);
				Text->SetSize(mRemoveLayerButton->GetSize());
				Text->SetPos(0.f, 0.f);

				Text->SetText(TEXT("X"));
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(BoxFontSize);

				Text->SetAlignH(ETextAlignH::Center);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CTileLayerEditorWidget::SetParentEditor(std::shared_ptr<CTileEditorWidget> ParentEditor)
{
	mParentEditor = ParentEditor;

	if (mResizeButton != nullptr)
	{
		mResizeButton->SetEventCallback(EButtonEventState::Click, [this, WeakParent = mParentEditor]() {
			std::shared_ptr<CTileEditorWidget> Parent = WeakParent.lock();
			if (Parent != nullptr)
			{
				Parent->OnClickResizeLayer(GetLayerName(), std::static_pointer_cast<CTileLayerEditorWidget>(mSelf.lock()));
			}
			});
	}
	if (mEnableButton != nullptr)
	{
		mEnableButton->SetEventCallback(EButtonEventState::Click, [this, WeakParent = mParentEditor]() {
			std::shared_ptr<CTileEditorWidget> Parent = WeakParent.lock();
			if (Parent != nullptr)
			{
				Parent->OnClickEnableLayer(GetLayerName(), std::static_pointer_cast<CTileLayerEditorWidget>(mSelf.lock()));
			}
			});
	}
	if (mRenderLayerUpButton != nullptr)
	{
		mRenderLayerUpButton->SetEventCallback(EButtonEventState::Click, [this, WeakParent = mParentEditor]() {
			std::shared_ptr<CTileEditorWidget> Parent = WeakParent.lock();
			if (Parent != nullptr)
			{
				Parent->OnClickRenderLayerUp(GetLayerName(), std::static_pointer_cast<CTileLayerEditorWidget>(mSelf.lock()));
			}
			});
	}
	if (mRenderLayerDownButton != nullptr)
	{
		mRenderLayerDownButton->SetEventCallback(EButtonEventState::Click, [this, WeakParent = mParentEditor]() {
			std::shared_ptr<CTileEditorWidget> Parent = WeakParent.lock();
			if (Parent != nullptr)
			{
				Parent->OnClickRenderLayerDown(GetLayerName(), std::static_pointer_cast<CTileLayerEditorWidget>(mSelf.lock()));
			}
			});
	}
	if (mRemoveLayerButton != nullptr)
	{
		mRemoveLayerButton->SetEventCallback(EButtonEventState::Click, [this, WeakParent = mParentEditor]() {
			std::shared_ptr<CTileEditorWidget> Parent = WeakParent.lock();
			if (Parent != nullptr)
			{
				Parent->OnClickRemoveLayer(GetLayerName(), std::static_pointer_cast<CTileLayerEditorWidget>(mSelf.lock()));
			}
			});
	}
}

void CTileLayerEditorWidget::UpdateLayerNameText()
{
	std::string LayerName = GetLayerName();
	mLayerNameText->SetText((TEXT("Layer ") + StringUtils::ConvertUtf8ToWString(LayerName)).c_str());
}

void CTileLayerEditorWidget::UpdateRenderLayerNameText()
{
	std::string RenderLayerName = GetRenderLayerName();
	mRenderLayerNameText->SetText((TEXT("Render ") + StringUtils::ConvertUtf8ToWString(RenderLayerName)).c_str());
}

void CTileLayerEditorWidget::UpdateEnableText()
{
	mEnableText->SetText(
		(mIsEnable == true) ? TEXT("SHOW") : TEXT("HIDE")
	);
	mEnableText->SetTextColor(
		(mIsEnable == true) ? FVector4::Green : FVector4::Red
	);
}
