#include "Pch.h"
#include "LoadWidget.h"

#include "Device.h"
#include "World/World.h"
#include "World/WorldManager.h"
#include "World/CameraManager.h"

#include "UI/Image.h"

bool CLoadWidget::Init()
{
	CWidgetContainer::Init();

	FResolution	RS = CDevice::GetInst()->GetResolution();
	float ZoomRate = mWorld.lock()->GetCameraManager().lock()->GetMainCameraZoomRate();

	std::shared_ptr<CImage> PanelImage = CreateWidget<CImage>("PanelImage", 0).lock();
	{
		PanelImage->SetUseInheritedColor(false);
		PanelImage->SetWidgetColor(FVector4::Black);
		PanelImage->SetPos(0.f, 0.f);
		PanelImage->SetPivot(0.f, 0.f);
		PanelImage->SetSize(RS.Width * ZoomRate, RS.Height * ZoomRate);
	}

	return true;
}
