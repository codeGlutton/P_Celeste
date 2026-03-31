#include "Pch.h"
#include "TitleWidget.h"

#include "Engine.h"
#include "Device.h"
#include "World/World.h"
#include "World/WorldManager.h"
#include "World/CameraManager.h"

#include "World/LoadWorld.h"

#include "UI/Image.h"
#include "UI/Button.h"
#include "UI/TextBlock.h"

CTitleWidget::CTitleWidget()
{
}

CTitleWidget::~CTitleWidget()
{
}

bool CTitleWidget::Init()
{
	CWidgetContainer::Init();

	FResolution	RS = CDevice::GetInst()->GetResolution();
	float ZoomRate = mWorld.lock()->GetCameraManager().lock()->GetMainCameraZoomRate();

	float ButtonTotalRateY = mClimbButtonSizeRate.y + mMainButtons.size() * mMainButtonSizeRate.y;
	ButtonTotalRateY += (mMainButtons.size() - 1) * mMainButtonLineHeightRateY;

	FVector3 ButtonStartPos;
	ButtonStartPos.x = RS.Width * ZoomRate * mAllButtonOffsetRateX;
	ButtonStartPos.y = RS.Height * ZoomRate * (1.f - ButtonTotalRateY) * 0.5f;

	InitBlindImages(RS, ZoomRate);
	{
		std::shared_ptr<CImage> BackGround = CreateWidget<CImage>("Background").lock();

		BackGround->SetSize(RS.Width * ZoomRate, RS.Height * ZoomRate);
		BackGround->SetTexture("T_TestBackground");
	}

	InitClimbButton(RS, ZoomRate, ButtonStartPos);

#ifdef _DEBUG

	InitMainButton(RS, ZoomRate, ETitleButtonType::Editor, ButtonStartPos, &CTitleWidget::OnClickEditor, "T_MainEditorImage", TEXT("에디터"));

#endif // _DEBUG

	InitMainButton(RS, ZoomRate, ETitleButtonType::Exit, ButtonStartPos, &CTitleWidget::OnClickExit, "T_MainExitImage", TEXT("종료"));

	return true;
}

void CTitleWidget::Update(float DeltaTime)
{
	CWidgetContainer::Update(DeltaTime);

	UpdateScreenAnimTime(DeltaTime);

	UpdateButtonAnimTimes(DeltaTime);
	UpdateClimbButtonAnim();
	UpdateMainButtonAnims();
}

void CTitleWidget::Render()
{
	CWidgetContainer::Render();
}

void CTitleWidget::OnClickClimb()
{
	mScreenChangeType = ETitleScreenChangeType::Select;
	mOnFinishScreenAnim = [this]() {
		auto World = CWorldManager::GetInst()->CreateWorld<CLoadWorld>(true).lock();
		World->Load(EWorldType::MainStage0);
		};
}

void CTitleWidget::OnClickEditor()
{
	mScreenChangeType = ETitleScreenChangeType::Select;
	mOnFinishScreenAnim = [this]() {
		auto World = CWorldManager::GetInst()->CreateWorld<CLoadWorld>(true).lock();
		World->Load(EWorldType::Editor);
		};
}

void CTitleWidget::OnClickExit()
{
	mScreenChangeType = ETitleScreenChangeType::Exit;
	mOnFinishScreenAnim = [this]() {
		CEngine::GetInst()->Destroy();
		};
}

void CTitleWidget::OnHoverButton(ETitleButtonType::Type Type)
{
	mHoveredButtonType = Type;
}

void CTitleWidget::OnUnhoverButton(ETitleButtonType::Type Type)
{
	mHoveredButtonType = ETitleButtonType::None;
}

void CTitleWidget::InitBlindImages(const FResolution& RS, float ZoomRate)
{
	FVector2 InOutBlindSize = { RS.Width * ZoomRate, RS.Height * ZoomRate };
	{
		std::shared_ptr<CImage> Image = CreateWidget<CImage>("InOutBlind", 2).lock();
		mInOutBlindImage = Image;

		Image->SetEnable(true);

		Image->SetUseInheritedColor(false);
		Image->SetPivot(0.f, 0.f);
		Image->SetPos(0.f, 0.f);
		Image->SetSize(InOutBlindSize);

		Image->SetWidgetColor(FVector4::Black);
	}

	std::shared_ptr<CWidgetContainer> SelectBlindContainer = CreateWidget<CWidgetContainer>("SelectBlindContainer", 2).lock();
	{
		mSelectBlindContainer = SelectBlindContainer;

		FVector2 SelectBlindSize = { RS.Width * ZoomRate * 2.f, RS.Height * ZoomRate / 6.f };
		
		SelectBlindContainer->SetEnable(false);

		SelectBlindContainer->SetUseInheritedColor(false);
		SelectBlindContainer->SetPivot(0.f, 0.f);
		SelectBlindContainer->SetPos(-SelectBlindSize.x, 0.f);

		for (int32 i = 0; i < mSelectBlindImages.size(); ++i)
		{
			std::shared_ptr<CImage> Image = SelectBlindContainer->CreateWidget<CImage>("SelectBlind" + std::to_string(i), 0).lock();
			mSelectBlindImages[i] = Image;

			Image->SetUseInheritedColor(false);
			Image->SetPivot(0.f, 0.f);
			Image->SetPos(-(i * SelectBlindSize.x * mSelectBlindStepRate * 0.5f), i * SelectBlindSize.y);
			Image->SetSize(SelectBlindSize);

			Image->SetWidgetColor(FVector4::Black);
			Image->SetTexture("T_ScreenChange");
		}
	}
}

void CTitleWidget::InitClimbButton(const FResolution& RS, float ZoomRate, const FVector3& ButtonStartPos)
{
	std::shared_ptr<CButton> ClimbButton = CreateWidget<CButton>("ClimbButton", 1).lock();
	{
		mMainButtons[ETitleButtonType::Climb] = ClimbButton;

		ClimbButton->SetPos(ButtonStartPos);
		ClimbButton->SetPivot(0.f, 0.f);
		ClimbButton->SetSize(RS.Width * ZoomRate * mClimbButtonSizeRate.x, RS.Height * ZoomRate * mClimbButtonSizeRate.y);
		ClimbButton->SetChildColor(EButtonState::Normal, FVector4::White);
		ClimbButton->SetOpacity(0.f);

		ClimbButton->SetEventCallback(EButtonEventState::Click, this, &CTitleWidget::OnClickClimb);
		ClimbButton->SetEventCallback(EButtonEventState::Hovered, [this]() {
			OnHoverButton(ETitleButtonType::Climb);
			});
		ClimbButton->SetEventCallback(EButtonEventState::Unhovered, [this]() {
			OnUnhoverButton(ETitleButtonType::Climb);
			});

		ClimbButton->SetSound(EButtonEventState::Click, "S_MainClimbSelect");
		ClimbButton->SetSound(EButtonEventState::Hovered, "S_MainRollUp");
		ClimbButton->SetSound(EButtonEventState::Unhovered, "S_MainRollDown");

		std::shared_ptr<CWidgetContainer> ClimbContainer = CreateStaticWidget<CWidgetContainer>("ClimbContainer", mWorld);
		ClimbButton->SetChild(ClimbContainer);
		{
			ClimbContainer->SetUseInheritedColor(true);
			ClimbContainer->SetPivot(0.f, 0.f);
			ClimbContainer->SetSize(ClimbButton->GetSize());

			float AccClimbBoxRateY = 0.f;
			std::shared_ptr<CImage> ClimbImg = ClimbContainer->CreateWidget<CImage>("ClimbImg", 0).lock();
			{
				const float ImageSize = std::min<float>(ClimbButton->GetSize().x, ClimbButton->GetSize().y * mClimbImageRateY);
				const float ImagePosX = (ClimbButton->GetSize().x - ImageSize) * 0.5f;

				ClimbImg->SetUseInheritedColor(false);
				ClimbImg->SetPivot(0.f, 0.f);
				ClimbImg->SetPos(ImagePosX, ClimbButton->GetSize().y * AccClimbBoxRateY);
				ClimbImg->SetSize(ImageSize, ImageSize);

				ClimbImg->SetTexture("T_MainClimbImage");

				AccClimbBoxRateY += mClimbImageRateY + mClimbImagePaddingRateY;
			}
			std::shared_ptr<CTextBlock> ClimbText = ClimbContainer->CreateWidget<CTextBlock>("ClimbText", 0).lock();
			{
				mMainTexts[ETitleButtonType::Climb] = ClimbText;

				const float FontSize = std::min<float>(ClimbButton->GetSize().x, ClimbButton->GetSize().y * mClimbTextRateY) * (1.f - mClimbTextPaddingRateY);

				ClimbText->SetUseInheritedColor(true);
				ClimbText->SetPivot(0.f, 0.f);
				ClimbText->SetPos(0.f, ClimbButton->GetSize().y * AccClimbBoxRateY);
				ClimbText->SetSize(ClimbButton->GetSize().x, ClimbButton->GetSize().y * mClimbTextRateY);

				ClimbText->SetUseInheritedTextColor(true);
				ClimbText->SetText(TEXT("오르기"));
				ClimbText->SetTextColor(FVector4::White);

				ClimbText->SetFont("EngineLight");
				ClimbText->SetFontSize(FontSize);

				ClimbText->EnableOutline(true);
				ClimbText->SetOutlineTextColor(FVector4::Black);
				ClimbText->SetOutlineWidth(1.2f);

				ClimbText->SetAlignH(ETextAlignH::Center);
				ClimbText->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CTitleWidget::InitMainButton(const FResolution& RS, float ZoomRate, ETitleButtonType::Type Type, const FVector3& ButtonStartPos, void(CTitleWidget::* ClickFunc)(), const char* TextureName, const TCHAR* InnerText)
{
	if (mMainButtons[Type].expired() == false)
	{
		return;
	}

	const int32 ButtonIndex = static_cast<int32>(Type);
	const float ButtonOffsetY = (RS.Height * ZoomRate *(mMainButtonSizeRate.y + mMainButtonLineHeightRateY)) * (ButtonIndex - 1) + (RS.Height * ZoomRate * (mClimbButtonSizeRate.y + mClimbButtonLineHeightRateY));
	std::shared_ptr<CButton> Button = CreateWidget<CButton>("MainButton" + std::to_string(ButtonIndex), 1).lock();
	{
		mMainButtons[Type] = Button;

		Button->SetPos(ButtonStartPos + FVector3(0.f, ButtonOffsetY, 0.f));
		Button->SetPivot(0.f, 0.f);
		Button->SetSize(RS.Width * ZoomRate * mMainButtonSizeRate.x, RS.Height * ZoomRate * mMainButtonSizeRate.y);
		Button->SetChildColor(EButtonState::Normal, FVector4::White);
		Button->SetOpacity(0.f);

		Button->SetEventCallback(EButtonEventState::Click, this, ClickFunc);
		Button->SetEventCallback(EButtonEventState::Hovered, [this, Type]() {
			OnHoverButton(Type);
			});
		Button->SetEventCallback(EButtonEventState::Unhovered, [this, Type]() {
			OnUnhoverButton(Type);
			});

		Button->SetSound(EButtonEventState::Click, "S_MainButtonSelect");
		Button->SetSound(EButtonEventState::Hovered, "S_MainRollUp");
		Button->SetSound(EButtonEventState::Unhovered, "S_MainRollDown");

		std::shared_ptr<CWidgetContainer> Container = CreateStaticWidget<CWidgetContainer>("MainContainer" + std::to_string(ButtonIndex), mWorld);
		Button->SetChild(Container);
		{
			Container->SetUseInheritedColor(true);
			Container->SetPivot(0.f, 0.f);
			Container->SetSize(Button->GetSize());

			float AccBoxRateX = 0.f;
			std::shared_ptr<CImage> Img = Container->CreateWidget<CImage>("MainImage" + std::to_string(ButtonIndex), 0).lock();
			{
				const float ImageSize = std::min<float>(Button->GetSize().x * mMainImageRateX, Button->GetSize().y);
				const float ImagePosY = (Button->GetSize().y - ImageSize) * 0.5f;

				Img->SetUseInheritedColor(false);
				Img->SetPivot(0.f, 0.f);
				Img->SetPos(Button->GetSize().x * AccBoxRateX, ImagePosY);
				Img->SetSize(ImageSize, ImageSize);

				Img->SetTexture(TextureName);

				AccBoxRateX += mMainImageRateX + mMainImagePaddingRateX;
			}
			std::shared_ptr<CTextBlock> Text = Container->CreateWidget<CTextBlock>("MainText" + std::to_string(ButtonIndex), 0).lock();
			{
				mMainTexts[ETitleButtonType::Climb] = Text;

				const float FontSize = std::min<float>(Button->GetSize().x * mMainTextRateX, Button->GetSize().y) * (1.f - mMainTextPaddingRateY);

				Text->SetUseInheritedColor(true);
				Text->SetPivot(0.f, 0.f);
				Text->SetPos(Button->GetSize().x * AccBoxRateX, 0.f);
				Text->SetSize(Button->GetSize().x * mMainTextRateX, Button->GetSize().y);

				Text->SetUseInheritedTextColor(true);
				Text->SetText(InnerText);
				Text->SetTextColor(FVector4::White);

				Text->SetFont("EngineLight");
				Text->SetFontSize(FontSize);

				Text->EnableOutline(true);
				Text->SetOutlineTextColor(FVector4::Black);
				Text->SetOutlineWidth(1.2f);

				Text->SetAlignH(ETextAlignH::Left);
				Text->SetAlignV(ETextAlignV::Middle);
			}
		}
	}
}

void CTitleWidget::UpdateScreenAnimTime(float DeltaTime)
{
	if (mScreenChangeType == ETitleScreenChangeType::Finish)
	{
		return;
	}

	std::shared_ptr<CImage> InOutImage = mInOutBlindImage.lock();
	std::shared_ptr<CWidgetContainer> Container = mSelectBlindContainer.lock();

	if (InOutImage->GetEnable() == false)
	{
		InOutImage->SetEnable(true);
	}

	bool IsFinish = false;
	if (mScreenChangeType == ETitleScreenChangeType::Select)
	{
		mAccSelectScreenAnimTime += DeltaTime;
		if (mAccSelectScreenAnimTime >= mScreenAnimMaxTime)
		{
			IsFinish = true;
			mAccSelectScreenAnimTime = mScreenAnimMaxTime;
		}

		if (Container->GetEnable() == false)
		{
			Container->SetEnable(true);
		}

		FVector3 NewAnimPos = MathUtils::Lerp(
			FVector3::Zero, 
			-Container->GetPos() * (1.f - mSelectBlindStepRate * mSelectBlindImages.size() * 0.5f),
			mAccSelectScreenAnimTime / mScreenAnimMaxTime
		);
		Container->SetAnimPos(NewAnimPos);
	}
	else
	{
		if (mScreenChangeType == ETitleScreenChangeType::Enter)
		{
			mAccInOutScreenAnimTime += DeltaTime;
			if (mAccInOutScreenAnimTime >= mScreenAnimMaxTime)
			{
				IsFinish = true;
				mAccInOutScreenAnimTime = mScreenAnimMaxTime;
			}
		}
		else
		{
			mAccInOutScreenAnimTime -= DeltaTime;
			if (mAccInOutScreenAnimTime <= 0.f)
			{
				IsFinish = true;
				mAccInOutScreenAnimTime = 0.f;
			}
		}

		InOutImage->SetOpacity(1.f - mAccInOutScreenAnimTime / mScreenAnimMaxTime);
	}

	if (IsFinish == true)
	{
		mScreenChangeType = ETitleScreenChangeType::Finish;

		InOutImage->SetEnable(false);
		Container->SetEnable(false);
		if (mOnFinishScreenAnim != nullptr)
		{
			mOnFinishScreenAnim();
		}
	}
}

void CTitleWidget::UpdateButtonAnimTimes(float DeltaTime)
{
	mAccHoveredButtonColorTime += DeltaTime;
	while (mAccHoveredButtonColorTime > mHoveredButtonColorMaxTime)
	{
		mAccHoveredButtonColorTime -= mHoveredButtonColorMaxTime;
		mHoveredColorIndex = (mHoveredColorIndex + 1) % mHoveredColor.size();
	}

	if (mHoveredButtonType != ETitleButtonType::None)
	{
		mAccButtonAnimTime[mHoveredButtonType] += DeltaTime;
		if (mAccButtonAnimTime[mHoveredButtonType] >= mButtonAnimMaxTime)
		{
			mAccButtonAnimTime[mHoveredButtonType] = mButtonAnimMaxTime;
		}

		std::shared_ptr<CButton> Button = mMainButtons[mHoveredButtonType].lock();
		Button->SetChildColor(EButtonState::Hovered, mHoveredColor[mHoveredColorIndex]);
		Button->SetChildColor(EButtonState::Click, mHoveredColor[mHoveredColorIndex]);
	}

	for (uint8 i = 0; i < ETitleButtonType::Count; ++i)
	{
		if (i == mHoveredButtonType)
		{
			continue;
		}

		mAccButtonAnimTime[i] -= DeltaTime;
		if (mAccButtonAnimTime[i] <= 0.f)
		{
			mAccButtonAnimTime[i] = 0.f;
		}
	}
}

void CTitleWidget::UpdateClimbButtonAnim()
{
	const float AnimRate = mAccButtonAnimTime[ETitleButtonType::Climb] / mButtonAnimMaxTime;

	std::shared_ptr<CButton> Button = mMainButtons[ETitleButtonType::Climb].lock();
	const FVector3 ControlPos = { 0.f, -Button->GetSize().y * 0.05f, 0.f };

	const FVector3 AnimOffset = MathUtils::BezierQuad(FVector3::Zero, ControlPos, FVector3::Zero, AnimRate);
	Button->GetChild()->SetAnimPos(AnimOffset);
}

void CTitleWidget::UpdateMainButtonAnims()
{
	for (uint8 i = ETitleButtonType::Climb + 1; i < ETitleButtonType::Count; ++i)
	{
		const float AnimRate = mAccButtonAnimTime[i] / mButtonAnimMaxTime;

		std::shared_ptr<CButton> Button = mMainButtons[i].lock();
		const FVector3 ControlPos = { Button->GetSize().x * 0.05f, -Button->GetSize().y * 0.1f, 0.f };
		const FVector3 EndPos = { Button->GetSize().x * 0.1f, 0.f, 0.f };

		const FVector3 AnimOffset = MathUtils::BezierQuad(FVector3::Zero, ControlPos, EndPos, AnimRate);
		Button->GetChild()->SetAnimPos(AnimOffset);
	}
}
