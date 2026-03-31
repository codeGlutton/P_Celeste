#include "Pch.h"
#include "TileMapCameraComponent.h"

#include "World/TileMainWorld.h"
#include "GameObject/Tile/MapInfo.h"

bool CTileMapCameraComponent::Init()
{
	CCameraComponent::Init();

	mPreClampCameraPos = GetWorldPos();

	return true;
}

void CTileMapCameraComponent::UpdateOnMain(float DeltaTime)
{
	SetRelativePos(mDesiredRelativePos);
	FVector3 ClampCameraPos = GetWorldPos();
	std::shared_ptr<CTileMainWorldBase> TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
	if (TileWorld != nullptr)
	{
		std::shared_ptr<FMapInfoBase> MapInfo = TileWorld->GetCurTileMapInfo().lock();
		if (MapInfo != nullptr)
		{
			ClampCameraPos = MapInfo->ClampCameraInCurTileMap(ClampCameraPos);
		}
	}

	ClampCameraPos = MathUtils::Lerp(mPreClampCameraPos, ClampCameraPos, mClampSmoothRate);
	SetWorldPos(ClampCameraPos);

	mPreClampCameraPos = ClampCameraPos;

	CCameraComponent::UpdateOnMain(DeltaTime);
}

void CTileMapCameraComponent::OnChangeToMainCamera()
{
	SetWorldPos(GetClampCameraPos());
	mPreClampCameraPos = GetWorldPos();
}

FVector3 CTileMapCameraComponent::GetClampCameraPos() const
{
	FVector3 ClampCameraPos = GetWorldPos();
	std::shared_ptr<CTileMainWorldBase> TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
	if (TileWorld != nullptr)
	{
		std::shared_ptr<FMapInfoBase> MapInfo = TileWorld->GetCurTileMapInfo().lock();
		if (MapInfo != nullptr)
		{
			ClampCameraPos = MapInfo->ClampCameraInCurTileMap(ClampCameraPos);
		}
	}
	return ClampCameraPos;
}

CTileMapCameraComponent* CTileMapCameraComponent::Clone() const
{
	return new CTileMapCameraComponent(*this);
}
