#include "Pch.h"
#include "MapInfo.h"

#include "GameObject/Tile/StaticTileMapObject.h"

int32 FMapInfoBase::mMapMaxIndex = 0;

std::shared_ptr<FMapInfoBase> FMapInfoBase::ConnectMap(EMapMoveDirection::Type ConnectDir, std::shared_ptr<FMapInfoBase> MapInfo)
{
	mNextMapInfos[ConnectDir] = MapInfo;

	std::shared_ptr<FMapInfoBase> SelfInfo = shared_from_this();
	MapInfo->mNextMapInfos[EMapMoveDirection::ReverseType(ConnectDir)] = SelfInfo;
	return SelfInfo;
}

void FMapInfoBase::StartMap(std::shared_ptr<CWorld>& World, const FVector3& Pos)
{
	std::shared_ptr<CStaticTileMapObject> SpawnedMap = SpawnMap(World);
	if (SpawnedMap == nullptr)
	{
		return;
	}
	SpawnedMap->SetWorldPos(Pos + mSpawnOffset);

	for (uint8 i = 0; i < EMapMoveDirection::Count; ++i)
	{
		std::shared_ptr<FMapInfoBase> MapInfo = mNextMapInfos[i].lock();
		if (MapInfo == nullptr)
		{
			continue;
		}

		std::shared_ptr<CStaticTileMapObject> MoveNearByMap = MapInfo->SpawnMap(World);
		FVector3 MoveNearByMapPos = FindMapSpawnPos(
			SpawnedMap->GetWorldPos() - mSpawnOffset,
			SpawnedMap->GetTileMapMaxSize(),
			MoveNearByMap->GetTileMapMaxSize(),
			static_cast<EMapMoveDirection::Type>(i)
		);
		MoveNearByMap->SetWorldPos(MoveNearByMapPos + MapInfo->mSpawnOffset);
	}

	/* 이벤트 */

	for (auto& Event : SpawnedMap->mOnEnterMap)
	{
		Event();
	}
}

std::shared_ptr<FMapInfoBase> FMapInfoBase::MoveMap(std::shared_ptr<CWorld>& World, EMapMoveDirection::Type MoveDir)
{
	std::shared_ptr<CStaticTileMapObject> SpawnedMap = mSpawnedMap.lock();
	if (SpawnedMap == nullptr)
	{
		return nullptr;
	}

	std::shared_ptr<FMapInfoBase> MoveMapInfo = mNextMapInfos[MoveDir].lock();
	if (MoveMapInfo == nullptr)
	{
		return nullptr;
	}

	SpawnedMap->SaveMapRuntimeData(mMapRuntimeData);

	/* 제거될 맵 필터링 및 다음 근처 맵 생성 */

	std::unordered_set<FMapInfoBase*> DestroyMaps;
	for (auto& CurNearByMapInfo : this->mNextMapInfos)
	{
		std::shared_ptr<FMapInfoBase> MapInfo = CurNearByMapInfo.lock();
		if (MapInfo == nullptr)
		{
			continue;
		}
		DestroyMaps.insert(MapInfo.get());
	}
	DestroyMaps.erase(MoveMapInfo.get());

	std::shared_ptr<CStaticTileMapObject> MoveSpawnMap = MoveMapInfo->mSpawnedMap.lock();
	for (uint8 i = 0; i < EMapMoveDirection::Count; ++i)
	{
		std::shared_ptr<FMapInfoBase> MoveNearByMapInfo = MoveMapInfo->mNextMapInfos[i].lock();
		if (MoveNearByMapInfo == nullptr)
		{
			continue;
		}

		DestroyMaps.erase(MoveNearByMapInfo.get());
		if (MoveNearByMapInfo->mSpawnedMap.lock() == nullptr)
		{
			std::shared_ptr<CStaticTileMapObject> MoveNearByMap = MoveNearByMapInfo->SpawnMap(World);
			FVector3 MoveNearByMapPos = FindMapSpawnPos(
				MoveSpawnMap->GetWorldPos() - MoveMapInfo->mSpawnOffset,
				MoveSpawnMap->GetTileMapMaxSize(),
				MoveNearByMap->GetTileMapMaxSize(),
				static_cast<EMapMoveDirection::Type>(i)
			);
			MoveNearByMap->SetWorldPos(MoveNearByMapPos + MoveNearByMapInfo->mSpawnOffset);
		}
	}

	/* 맵 제거 */

	for (auto& DestroyMap : DestroyMaps)
	{
		DestroyMap->DestroyMap();
	}

	/* 이벤트 */

	for (auto& Event : SpawnedMap->mOnLeaveMap)
	{
		Event();
	}
	for (auto& Event : MoveSpawnMap->mOnEnterMap)
	{
		Event();
	}

	return MoveMapInfo;
}

bool FMapInfoBase::IsInCurTileMap(const FVector2& Pos) const
{
	std::shared_ptr<CStaticTileMapObject> SpawnedMap = mSpawnedMap.lock();
	if (SpawnedMap == nullptr)
	{
		return false;
	}
	return SpawnedMap->IsInTileMap(Pos);
}

bool FMapInfoBase::IsInCurTileMap(const FVector2& Pos, OUT EMapMoveDirection::Type& OutDir) const
{
	std::shared_ptr<CStaticTileMapObject> SpawnedMap = mSpawnedMap.lock();
	if (SpawnedMap == nullptr)
	{
		return false;
	}
	return SpawnedMap->IsInTileMap(Pos, OUT OutDir);
}

FVector3 FMapInfoBase::ClampCameraInCurTileMap(const FVector3& Pos) const
{
	std::shared_ptr<CStaticTileMapObject> SpawnedMap = mSpawnedMap.lock();
	if (SpawnedMap == nullptr)
	{
		return FVector3::Zero;
	}
	return SpawnedMap->ClampCameraInTileMap(Pos);
}

void FMapInfoBase::RespawnTargetObject(EMapMoveDirection::Type Dir, std::weak_ptr<CGameObject> TargetObject) const
{
	std::shared_ptr<CStaticTileMapObject> SpawnedMap = mSpawnedMap.lock();
	std::shared_ptr<CGameObject> TargetObj = TargetObject.lock();
	if (SpawnedMap == nullptr || TargetObj == nullptr)
	{
		return;
	}

	SpawnedMap->LoadMapRuntimeData(mMapRuntimeData);
	TargetObj->SetWorldPos(SpawnedMap->GetPlayerRespawnPos(Dir));
}

std::shared_ptr<CStaticTileMapObject> FMapInfoBase::SpawnMap(std::shared_ptr<CWorld>& World)
{
	std::shared_ptr<CStaticTileMapObject> TileMapObject = mSpawnedMap.lock();
	if (TileMapObject == nullptr)
	{
		mSpawnedMap = TileMapObject = CreateMap(World);
		if (mMapRuntimeData == nullptr)
		{
			mMapRuntimeData = CreateMapRuntimeData();
		}
		TileMapObject->LoadMapRuntimeData(mMapRuntimeData);
	}
	return TileMapObject;
}

void FMapInfoBase::DestroyMap()
{
	std::shared_ptr<CStaticTileMapObject> TileMapObject = mSpawnedMap.lock();
	if (TileMapObject == nullptr)
	{
		if (mMapRuntimeData == nullptr)
		{
			mMapRuntimeData = CreateMapRuntimeData();
		}
		TileMapObject->SaveMapRuntimeData(mMapRuntimeData);
	}
	TileMapObject->Destroy();
	mSpawnedMap.reset();
}

FVector3 FMapInfoBase::FindMapSpawnPos(const FVector3& Pos, const FVector2& CurSize, const FVector3& NextSize, EMapMoveDirection::Type MoveDir)
{
	FVector3 ResultPos = Pos;
	switch (MoveDir)
	{
	case EMapMoveDirection::Up:
		ResultPos.y += CurSize.y;
		break;
	case EMapMoveDirection::Down:
		ResultPos.y -= NextSize.y;
		break;
	case EMapMoveDirection::Left:
		ResultPos.x -= NextSize.x;
		break;
	case EMapMoveDirection::Right:
		ResultPos.x += CurSize.x;
		break;
	case EMapMoveDirection::UpRight:
		ResultPos.x += CurSize.x;
		ResultPos.y += CurSize.y;
		break;
	case EMapMoveDirection::UpLeft:
		ResultPos.x -= NextSize.x;
		ResultPos.y += CurSize.y;
		break;
	case EMapMoveDirection::DownRight:
		ResultPos.x += CurSize.x;
		ResultPos.y -= NextSize.y;
		break;
	case EMapMoveDirection::DownLeft:
		ResultPos.x -= NextSize.x;
		ResultPos.y -= NextSize.y;
		break;
	}
	return ResultPos;
}