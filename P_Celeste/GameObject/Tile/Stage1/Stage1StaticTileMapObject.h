#pragma once

#include "GameObject/Tile/MapInfo.h"
#include "GameObject/Tile/StaticTileMapObject.h"
#include "GameObject/Tile/Shared/DestructableTileMapObject.h"
#include "GameObject/Mesh/SpringMapObject.h"
#include "GameObject/Item/StrawberryObject.h"

class CColliderBox2D;

class CStage1StaticTileMap0Object : public CStaticTileMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
	};

protected:
	CStage1StaticTileMap0Object();
	CStage1StaticTileMap0Object(const CStage1StaticTileMap0Object& ref) = default;
	CStage1StaticTileMap0Object(CStage1StaticTileMap0Object&& ref) noexcept = default;

public:
	virtual ~CStage1StaticTileMap0Object() override = default;

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
	bool InitBlockers();
	bool InitDynamicObjects();
};

class CStage1StaticTileMap1Object : public CStaticTileMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		bool mGetStrawberry0 = false;
	};

protected:
	CStage1StaticTileMap1Object();
	CStage1StaticTileMap1Object(const CStage1StaticTileMap1Object& ref) = default;
	CStage1StaticTileMap1Object(CStage1StaticTileMap1Object&& ref) noexcept = default;

public:
	virtual ~CStage1StaticTileMap1Object() override = default;

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
	bool InitBlockers();
	bool InitDynamicObjects();

protected:
	std::weak_ptr<CStrawberryObject> mStrawberry0;
};

class CStage1StaticTileMap2Object : public CStaticTileMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		bool mGetStrawberry1 = false;
	};

protected:
	CStage1StaticTileMap2Object();
	CStage1StaticTileMap2Object(const CStage1StaticTileMap2Object& ref) = default;
	CStage1StaticTileMap2Object(CStage1StaticTileMap2Object&& ref) noexcept = default;

public:
	virtual ~CStage1StaticTileMap2Object() override = default;

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
	bool InitBlockers();
	bool InitDynamicObjects();

protected:
	std::weak_ptr<CStrawberryObject> mStrawberry1;
};
