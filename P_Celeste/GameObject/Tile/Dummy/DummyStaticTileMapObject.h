#pragma once

#include "GameObject/Tile/MapInfo.h"
#include "GameObject/Tile/StaticTileMapObject.h"
#include "GameObject/Mesh/TriggerOneTimeDropMapObject.h"

class CDummyStaticTileMapObject : public CStaticTileMapObject
{
	friend class CWorld;
	friend class CObject;

private:
	static constexpr int32 mDynamicBlockCount = 8;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		std::array<CTriggerOneTimeDropMapObject::FMapRuntimeData, mDynamicBlockCount> mBlockDatas;
	};

protected:
	CDummyStaticTileMapObject();
	CDummyStaticTileMapObject(const CDummyStaticTileMapObject& ref) = default;
	CDummyStaticTileMapObject(CDummyStaticTileMapObject&& ref) noexcept = default;

public:
	virtual ~CDummyStaticTileMapObject() override = default;

public:
	virtual bool Init() override;

public:
	virtual void LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) override;
	virtual void SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const override;

protected:
	bool InitMainTileMap();
	bool InitCollisions();
	bool InitPlatforms();
	bool InitEnvMeshes();
	bool InitDynamicObjects();

protected:
	std::array<std::weak_ptr<CTriggerOneTimeDropMapObject>, mDynamicBlockCount> mDynamicBlocks;
};
