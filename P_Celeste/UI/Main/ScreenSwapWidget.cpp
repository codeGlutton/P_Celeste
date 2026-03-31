#include "Pch.h"
#include "ScreenSwapWidget.h"

#include "Device.h"
#include "World/World.h"
#include "World/CameraManager.h"

#include "UI/Image.h"

bool CScreenSwapWidget::Init()
{
	CWidgetContainer::Init();

	FResolution	RS = CDevice::GetInst()->GetResolution();
	float ZoomRate = mWorld.lock()->GetCameraManager().lock()->GetMainCameraZoomRate();

	InitBlindImages(RS, ZoomRate);

	return true;
}

void CScreenSwapWidget::Update(float DeltaTime)
{
	CWidgetContainer::Update(DeltaTime);

	UpdateScreenAnimTime(DeltaTime);
}

void CScreenSwapWidget::Open()
{
	mAccScreenAnimTime = 0.f;
	mSwapState = EScreenSwapState::Enter;
	std::shared_ptr<CWidgetContainer> Container = mBlindContainer.lock();
	Container->SetEnable(true);
	Container->SetAnimPos(-Container->GetPos() * (1.f - mSelectBlindStepRate * mBlindImages.size() * 0.5f));
}

void CScreenSwapWidget::Close()
{
	mAccScreenAnimTime = 0.f;
	mSwapState = EScreenSwapState::Leave;
	std::shared_ptr<CWidgetContainer> Container = mBlindContainer.lock();
	Container->SetEnable(true);
	Container->SetAnimPos(FVector3::Zero);
}

void CScreenSwapWidget::InitBlindImages(const FResolution& RS, float ZoomRate)
{
	std::shared_ptr<CWidgetContainer> BlindContainer = CreateWidget<CWidgetContainer>("BlindContainer", 2).lock();
	{
		mBlindContainer = BlindContainer;

		FVector2 SelectBlindSize = { RS.Width * ZoomRate * 2.f, RS.Height * ZoomRate / 6.f };

		BlindContainer->SetUseInheritedColor(false);
		BlindContainer->SetPivot(0.f, 0.f);
		BlindContainer->SetPos(-SelectBlindSize.x, 0.f);
		BlindContainer->SetAnimPos(BlindContainer->GetPos() * (1.f - mSelectBlindStepRate * mBlindImages.size() * 0.5f));

		for (int32 i = 0; i < mBlindImages.size(); ++i)
		{
			std::shared_ptr<CImage> Image = BlindContainer->CreateWidget<CImage>("Blind" + std::to_string(i), 0).lock();
			mBlindImages[i] = Image;

			Image->SetUseInheritedColor(false);
			Image->SetPivot(0.f, 0.f);
			Image->SetPos(-(i * SelectBlindSize.x * mSelectBlindStepRate * 0.5f), i * SelectBlindSize.y);
			Image->SetSize(SelectBlindSize);

			Image->SetWidgetColor(FVector4::Black);
			Image->SetTexture("T_ScreenChange");
		}
	}
}

void CScreenSwapWidget::UpdateScreenAnimTime(float DeltaTime)
{
	if (mSwapState == EScreenSwapState::None || mSwapState == EScreenSwapState::Exit)
	{
		return;
	}

	std::shared_ptr<CWidgetContainer> Container = mBlindContainer.lock();
	bool IsFinish = false;

	if (Container->GetEnable() == false)
	{
		Container->SetEnable(true);
	}
	mAccScreenAnimTime += DeltaTime;
	if (mSwapState == EScreenSwapState::Enter)
	{
		if (mAccScreenAnimTime >= mScreenAnimMaxTime)
		{
			IsFinish = true;
			mSwapState = EScreenSwapState::None;
			mAccScreenAnimTime = mScreenAnimMaxTime;
		}

		FVector3 NewAnimPos = MathUtils::Lerp(
			-Container->GetPos() * (1.f - mSelectBlindStepRate * mBlindImages.size() * 0.5f),
			-Container->GetPos() * (1.f - mSelectBlindStepRate * mBlindImages.size() * 0.5f) * 2.5f,
			mAccScreenAnimTime / mScreenAnimMaxTime
		);
		Container->SetAnimPos(NewAnimPos);
	}
	else
	{
		if (mAccScreenAnimTime >= mScreenAnimMaxTime)
		{
			IsFinish = true;
			mSwapState = EScreenSwapState::Exit;
			mAccScreenAnimTime = mScreenAnimMaxTime;
		}

		FVector3 NewAnimPos = MathUtils::Lerp(
			FVector3::Zero,
			-Container->GetPos() * (1.f - mSelectBlindStepRate * mBlindImages.size() * 0.5f),
			mAccScreenAnimTime / mScreenAnimMaxTime
		);
		Container->SetAnimPos(NewAnimPos);
	}

	if (IsFinish == true)
	{
		if (mOnFinishScreenAnim != nullptr)
		{
			mOnFinishScreenAnim();
			mOnFinishScreenAnim = nullptr;
		}
	}
}