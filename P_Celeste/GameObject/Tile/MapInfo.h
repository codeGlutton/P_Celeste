#pragma once

#include "World/World.h"

class CWorld;
class CStaticTileMapObject;
class CGameObject;

namespace EMapMoveDirection
{
    enum Type : uint8
    {
        Up,
        Down,
        Left,
        Right,
        UpRight,
        UpLeft,
        DownRight,
        DownLeft,
        Count
    };

	inline Type ReverseType(Type Type)
	{
		switch (Type)
		{
		case EMapMoveDirection::Up:
			Type = EMapMoveDirection::Down;
			break;
		case EMapMoveDirection::Down:
			Type = EMapMoveDirection::Up;
			break;
		case EMapMoveDirection::Left:
			Type = EMapMoveDirection::Right;
			break;
		case EMapMoveDirection::Right:
			Type = EMapMoveDirection::Left;
			break;
		case EMapMoveDirection::UpRight:
			Type = EMapMoveDirection::DownLeft;
			break;
		case EMapMoveDirection::UpLeft:
			Type = EMapMoveDirection::DownRight;
			break;
		case EMapMoveDirection::DownRight:
			Type = EMapMoveDirection::UpLeft;
			break;
		case EMapMoveDirection::DownLeft:
			Type = EMapMoveDirection::UpRight;
			break;
		}
		return Type;
	}
}

struct FMapRuntimeDataBase
{
	virtual ~FMapRuntimeDataBase() = default;
};

struct FMapInfoBase : public std::enable_shared_from_this<FMapInfoBase>
{
protected:
	FMapInfoBase(const FVector2& SpawnOffset) :
		mSpawnOffset(SpawnOffset)
	{
	}

public:
	virtual ~FMapInfoBase() = default;

public:
	std::shared_ptr<FMapInfoBase> ConnectMap(EMapMoveDirection::Type ConnectDir, std::shared_ptr<FMapInfoBase> MapInfo);
	void StartMap(std::shared_ptr<CWorld>& World, const FVector3& Pos);
	std::shared_ptr<FMapInfoBase> MoveMap(std::shared_ptr<CWorld>& World, EMapMoveDirection::Type MoveDir);

public:
	bool IsInCurTileMap(const FVector2& Pos) const;
	bool IsInCurTileMap(const FVector2& Pos, OUT EMapMoveDirection::Type& OutDir) const;
	FVector3 ClampCameraInCurTileMap(const FVector3& Pos) const;

public:
	void RespawnTargetObject(EMapMoveDirection::Type Dir, std::weak_ptr<CGameObject> TargetObject) const;

protected:
	std::shared_ptr<CStaticTileMapObject> SpawnMap(std::shared_ptr<CWorld>& World);
	void DestroyMap();

private:
	virtual std::shared_ptr<CStaticTileMapObject> CreateMap(std::shared_ptr<CWorld>& World) = 0;
	virtual std::shared_ptr<FMapRuntimeDataBase> CreateMapRuntimeData() = 0;

protected:
	FVector3 FindMapSpawnPos(const FVector3& Pos, const FVector2& CurSize, const FVector3& NextSize, EMapMoveDirection::Type MoveDir);

protected:
	std::weak_ptr<CStaticTileMapObject> mSpawnedMap;
	std::shared_ptr<FMapRuntimeDataBase> mMapRuntimeData;

protected:
	const FVector2 mSpawnOffset;
	std::array<std::weak_ptr<FMapInfoBase>, EMapMoveDirection::Count> mNextMapInfos = {};

protected:
	static int32 mMapMaxIndex;
};

template<typename T>
struct FMapInfo : FMapInfoBase
{
public:
	FMapInfo(const FVector2& SpawnOffset) :
		FMapInfoBase(SpawnOffset)
	{
	}

private:
	virtual std::shared_ptr<CStaticTileMapObject> CreateMap(std::shared_ptr<CWorld>& World) override
	{
		std::shared_ptr<CStaticTileMapObject> TileMapObject = World->CreateGameObject<T>(
			"Map" + std::to_string(FMapInfoBase::mMapMaxIndex)
		).lock();
		++FMapInfoBase::mMapMaxIndex;

		return TileMapObject;
	}
	virtual std::shared_ptr<FMapRuntimeDataBase> CreateMapRuntimeData() override
	{
		return std::make_shared<typename T::FMapRuntimeData>();
	}
};



