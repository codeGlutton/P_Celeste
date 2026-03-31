#pragma once

#include "GameObject/Tile/MapInfo.h"
#include "GameObject/Tile/LayeredTileMapObject.h"
#include "GameObject/Interator/PlayerInteractableObject.h"

class CStaticTileMapObject : public CLayeredTileMapObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CStaticTileMapObject() = default;
	CStaticTileMapObject(const CStaticTileMapObject& ref) = default;
	CStaticTileMapObject(CStaticTileMapObject&& ref) noexcept = default;

public:
	virtual ~CStaticTileMapObject() override = default;

public:
	virtual bool Init() override;
	virtual void Destroy() override;

public:
	virtual void LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData);
	virtual void SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const;

public:
	bool IsInTileMap(const FVector2& Pos);
	bool IsInTileMap(const FVector2& Pos, OUT EMapMoveDirection::Type& OutDir);
	FVector3 ClampCameraInTileMap(const FVector3& Pos);

public:
	FVector3 GetPlayerRespawnPos(EMapMoveDirection::Type Dir) const
	{
		return GetWorldPos() + mPlayerRespawnOffsets[Dir];
	}

protected:
	void AddSubGameObject(const std::string& Key, std::weak_ptr<CGameObject> GameObject, bool mNeedAttach = true);
	std::weak_ptr<CGameObject> FindSubGameObject(const std::string& Key);
	std::weak_ptr<const CGameObject> FindSubGameObject(const std::string& Key) const;

private:
	std::unordered_map<std::string, std::weak_ptr<CGameObject>> mSubGameObjects;

protected:
	std::array<FVector3, EMapMoveDirection::Count> mPlayerRespawnOffsets = {};

public:
	std::vector<std::function<void()>> mOnEnterMap;
	std::vector<std::function<void()>> mOnLeaveMap;
};

