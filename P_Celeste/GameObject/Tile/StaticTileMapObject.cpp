#include "Pch.h"
#include "StaticTileMapObject.h"

#include "Device.h"
#include "World/World.h"
#include "World/CameraManager.h"

bool CStaticTileMapObject::Init()
{
	CLayeredTileMapObject::Init();

	return true;
}

void CStaticTileMapObject::Destroy()
{
	for (auto& SubObject : mSubGameObjects)
	{
		std::shared_ptr<CGameObject> GameObject = SubObject.second.lock();
		if (GameObject != nullptr)
		{
			GameObject->Destroy();
		}
	}
	CLayeredTileMapObject::Destroy();
}

void CStaticTileMapObject::LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData)
{
}

void CStaticTileMapObject::SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const
{
}

bool CStaticTileMapObject::IsInTileMap(const FVector2& Pos)
{
	const FVector3& WorldPos = GetWorldPos();
	FVector3 WorldMaxPos = WorldPos + GetTileMapMaxSize();
	if (Pos.x < WorldPos.x || Pos.x > WorldMaxPos.x || Pos.y < WorldPos.y || Pos.y > WorldMaxPos.y)
	{
		return false;
	}
	return true;
}

bool CStaticTileMapObject::IsInTileMap(const FVector2& Pos, OUT EMapMoveDirection::Type& OutDir)
{
	const FVector3& WorldPos = GetWorldPos();
	FVector3 WorldMaxPos = WorldPos + GetTileMapMaxSize();
	if (Pos.x < WorldPos.x)
	{
		if (Pos.y < WorldPos.y)
		{
			OutDir = EMapMoveDirection::DownLeft;
		}
		else if (Pos.y > WorldMaxPos.y)
		{
			OutDir = EMapMoveDirection::UpLeft;
		}
		else
		{
			OutDir = EMapMoveDirection::Left;
		}
		return false;
	}
	else if (Pos.x > WorldMaxPos.x)
	{
		if (Pos.y < WorldPos.y)
		{
			OutDir = EMapMoveDirection::DownRight;
		}
		else if (Pos.y > WorldMaxPos.y)
		{
			OutDir = EMapMoveDirection::UpRight;
		}
		else
		{
			OutDir = EMapMoveDirection::Right;
		}
		return false;
	}

	if (Pos.y < WorldPos.y)
	{
		OutDir = EMapMoveDirection::Down;
		return false;
	}
	else if (Pos.y > WorldMaxPos.y)
	{
		OutDir = EMapMoveDirection::Up;
		return false;
	}

	return true;
}

FVector3 CStaticTileMapObject::ClampCameraInTileMap(const FVector3& Pos)
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CCameraManager> CameraMgr = World->GetCameraManager().lock();

	const FResolution& RS = CDevice::GetInst()->GetResolution();
	const FVector2 HalfScreenSize = {
		RS.Width * 0.5f,
		RS.Height * 0.5f
	};
	const FVector3& WorldPos = GetWorldPos();
	FVector3 WorldMaxPos = WorldPos + GetTileMapMaxSize();
	
	FVector3 ResultPos = Pos;
	ResultPos.x = Clamp(Pos.x, WorldPos.x + HalfScreenSize.x, WorldMaxPos.x - HalfScreenSize.x);
	ResultPos.y = Clamp(Pos.y, WorldPos.y + HalfScreenSize.y, WorldMaxPos.y - HalfScreenSize.y);

	return ResultPos;
}

void CStaticTileMapObject::AddSubGameObject(const std::string& Key, std::weak_ptr<CGameObject> GameObject, bool mNeedAttach)
{
	if (mNeedAttach == true)
	{
		AttachGameObject(GameObject);
	}
	mSubGameObjects.insert({ Key, GameObject });
}

std::weak_ptr<CGameObject> CStaticTileMapObject::FindSubGameObject(const std::string& Key)
{
	if (mSubGameObjects.find(Key) == mSubGameObjects.end())
	{
		return std::weak_ptr<CGameObject>();
	}
	return mSubGameObjects.at(Key);
}

std::weak_ptr<const CGameObject> CStaticTileMapObject::FindSubGameObject(const std::string& Key) const
{
	if (mSubGameObjects.find(Key) == mSubGameObjects.end())
	{
		return std::weak_ptr<const CGameObject>();
	}
	return mSubGameObjects.at(Key);
}

