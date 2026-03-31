#include "Pch.h"
#include "EditorPlayer.h"

#include "Component/CameraComponent.h"
#include "Component/ObjectMovementComponent.h"

#include "Device.h"
#include "World/Input.h"
#include "World/WorldUIManager.h"

#include "World/World.h"
#include "UI/Editor/EditorWidget.h"
#include "UI/Editor/TileEditorWidget.h"

CEditorPlayer::CEditorPlayer()
{
	SetClassType<CEditorPlayer>();
}

void CEditorPlayer::Begin()
{
	CGameObject::Begin();

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldUIManager> UIMgr = World->GetUIManager().lock();

	mEditorWidget = UIMgr->FindWidget<CEditorWidget>("EditorWidget");
	std::shared_ptr<CEditorWidget> EditorWidget = mEditorWidget.lock();
	if (EditorWidget == nullptr)
	{
		return;
	}
}

bool CEditorPlayer::Init()
{
	CGameObject::Init();

	mCameraComponent = CreateComponent<CCameraComponent>("PlayerCamera");
	std::shared_ptr<CCameraComponent> CameraComp = mCameraComponent.lock();
	if (CameraComp != nullptr)
	{
		const FResolution& RS = CDevice::GetInst()->GetResolution();
		CameraComp->SetProjection(ECameraProjectionType::Ortho, 90.f, static_cast<float>(RS.Width), static_cast<float>(RS.Height), 1000.f);
		CameraComp->SetInheritRot(false);
	}

	mMovementComponent = CreateComponent<CObjectMovementComponent>("Movement");
	std::shared_ptr<CObjectMovementComponent> MovementComp = mMovementComponent.lock();
	if (MovementComp != nullptr)
	{
		MovementComp->SetUpdateComponent(mCameraComponent);
		MovementComp->SetSpeed(400.f);
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	if (World != nullptr)
	{
		std::shared_ptr<CInput> InputManager = World->GetInput().lock();
		if (InputManager != nullptr)
		{
			BindPlayerInputs(InputManager);
		}
	}

	return true;
}

void CEditorPlayer::MoveForward()
{
	auto Camera = mCameraComponent.lock();
	Camera->Zoom(-CTimer::GetDeltaTime());
}

void CEditorPlayer::MoveBack()
{
	auto Camera = mCameraComponent.lock();
	Camera->Zoom(CTimer::GetDeltaTime());
}

void CEditorPlayer::MoveLeft()
{
	auto Movement = mMovementComponent.lock();
	Movement->AddMove(-GetAxis(EAxis::X));
}

void CEditorPlayer::MoveRight()
{
	auto Movement = mMovementComponent.lock();
	Movement->AddMove(GetAxis(EAxis::X));
}

void CEditorPlayer::MoveUp()
{
	auto Movement = mMovementComponent.lock();
	Movement->AddMove(GetAxis(EAxis::Y));
}

void CEditorPlayer::MoveDown()
{
	auto Movement = mMovementComponent.lock();
	Movement->AddMove(-GetAxis(EAxis::Y));
}

void CEditorPlayer::MoveClick()
{
	if (mEditorWidget.expired() == true)
	{
		return;
	}

	std::shared_ptr<CEditorWidget> EditWidget = mEditorWidget.lock();
	const std::shared_ptr<CTileEditorWidget>& TileEditWidget = EditWidget->GetTileEditWidget();
	if (TileEditWidget != nullptr)
	{
		TileEditWidget->CheckTileUnderMouse();
	}
}

void CEditorPlayer::MoveCopyClick()
{
	if (mEditorWidget.expired() == true)
	{
		return;
	}

	std::shared_ptr<CEditorWidget> EditWidget = mEditorWidget.lock();
	const std::shared_ptr<CTileEditorWidget>& TileEditWidget = EditWidget->GetTileEditWidget();
	if (TileEditWidget != nullptr)
	{
		TileEditWidget->CopyFrameAndRotZ();
	}
}

void CEditorPlayer::MovePasteClick()
{
	if (mEditorWidget.expired() == true)
	{
		return;
	}

	std::shared_ptr<CEditorWidget> EditWidget = mEditorWidget.lock();
	const std::shared_ptr<CTileEditorWidget>& TileEditWidget = EditWidget->GetTileEditWidget();
	if (TileEditWidget != nullptr)
	{
		TileEditWidget->PasteFrameAndRotZ();
	}
}

void CEditorPlayer::MoveTileFrameUp()
{
	if (mEditorWidget.expired() == true)
	{
		return;
	}

	std::shared_ptr<CEditorWidget> EditWidget = mEditorWidget.lock();
	const std::shared_ptr<CTileEditorWidget>& TileEditWidget = EditWidget->GetTileEditWidget();
	if (TileEditWidget != nullptr && TileEditWidget->GetEnable() == true)
	{
		TileEditWidget->OnClickNextTileFrame();
	}
}

void CEditorPlayer::MoveTileFrameDown()
{
	if (mEditorWidget.expired() == true)
	{
		return;
	}

	std::shared_ptr<CEditorWidget> EditWidget = mEditorWidget.lock();
	const std::shared_ptr<CTileEditorWidget>& TileEditWidget = EditWidget->GetTileEditWidget();
	if (TileEditWidget != nullptr && TileEditWidget->GetEnable() == true)
	{
		TileEditWidget->OnClickPreTileFrame();
	}
}

void CEditorPlayer::MoveTileOnOff()
{
	if (mEditorWidget.expired() == true)
	{
		return;
	}

	std::shared_ptr<CEditorWidget> EditWidget = mEditorWidget.lock();
	const std::shared_ptr<CTileEditorWidget>& TileEditWidget = EditWidget->GetTileEditWidget();
	if (TileEditWidget != nullptr && TileEditWidget->GetEnable() == true)
	{
		TileEditWidget->OnClickRenderTile();
	}
}

void CEditorPlayer::MoveTileLock()
{
	if (mEditorWidget.expired() == true)
	{
		return;
	}

	std::shared_ptr<CEditorWidget> EditWidget = mEditorWidget.lock();
	const std::shared_ptr<CTileEditorWidget>& TileEditWidget = EditWidget->GetTileEditWidget();
	if (TileEditWidget != nullptr && TileEditWidget->GetEnable() == true)
	{
		TileEditWidget->OnClickSelectionLock();
	}
}

void CEditorPlayer::BindPlayerInputs(std::shared_ptr<CInput>& InputManager)
{
	InputManager->AddBindKey("MoveForward", 'W');
	InputManager->SetBindFunction<CEditorPlayer>("MoveForward", EInputType::Hold, this, &CEditorPlayer::MoveForward);

	InputManager->AddBindKey("MoveBack", 'S');
	InputManager->SetBindFunction<CEditorPlayer>("MoveBack", EInputType::Hold, this, &CEditorPlayer::MoveBack);

	InputManager->AddBindKey("MoveLeft", 'A');
	InputManager->SetBindFunction<CEditorPlayer>("MoveLeft", EInputType::Hold, this, &CEditorPlayer::MoveLeft);

	InputManager->AddBindKey("MoveRight", 'D');
	InputManager->SetBindFunction<CEditorPlayer>("MoveRight", EInputType::Hold, this, &CEditorPlayer::MoveRight);

	InputManager->AddBindKey("MoveUp", VK_SPACE);
	InputManager->SetBindFunction<CEditorPlayer>("MoveUp", EInputType::Hold, this, &CEditorPlayer::MoveUp);

	InputManager->AddBindKey("MoveDown", VK_LCONTROL);
	InputManager->SetKeyCtrl("MoveDown", true);
	InputManager->SetBindFunction<CEditorPlayer>("MoveDown", EInputType::Hold, this, &CEditorPlayer::MoveDown);

	InputManager->AddBindKey("MoveClick", VK_LBUTTON);
	InputManager->SetBindFunction<CEditorPlayer>("MoveClick", EInputType::Press, this, &CEditorPlayer::MoveClick);

	InputManager->AddBindKey("MoveCopyClick", VK_MBUTTON);
	InputManager->SetBindFunction<CEditorPlayer>("MoveCopyClick", EInputType::Press, this, &CEditorPlayer::MoveCopyClick);

	InputManager->AddBindKey("MovePasteClick", VK_RBUTTON);
	InputManager->SetBindFunction<CEditorPlayer>("MovePasteClick", EInputType::Hold, this, &CEditorPlayer::MovePasteClick);

	InputManager->AddBindKey("MoveTileFrameUp", 'J');
	InputManager->SetBindFunction<CEditorPlayer>("MoveTileFrameUp", EInputType::Hold, this, &CEditorPlayer::MoveTileFrameUp);

	InputManager->AddBindKey("MoveTileFrameDown", 'H');
	InputManager->SetBindFunction<CEditorPlayer>("MoveTileFrameDown", EInputType::Hold, this, &CEditorPlayer::MoveTileFrameDown);

	InputManager->AddBindKey("MoveTileOnOff", 'K');
	InputManager->SetBindFunction<CEditorPlayer>("MoveTileOnOff", EInputType::Press, this, &CEditorPlayer::MoveTileOnOff);

	InputManager->AddBindKey("MoveTileLock", 'L');
	InputManager->SetBindFunction<CEditorPlayer>("MoveTileLock", EInputType::Press, this, &CEditorPlayer::MoveTileLock);
}
