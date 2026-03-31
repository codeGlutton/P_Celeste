#pragma once

#include "GameObject/Tile/MapInfo.h"
#include "GameObject/Tile/StaticTileMapObject.h"
#include "GameObject/Tile/Shared/TriggerDropTileMapObject.h"
#include "GameObject/Mesh/TriggerOneTimeDropMapObject.h"

class CBird;
class COldLady;
class CColliderBox2D;
class CWorldTimeChangableBoxCollider;

class CStage0StaticTileMap0Object : public CStaticTileMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		CTriggerDropTileMapObject::FMapRuntimeData mIceBlockData;
	};

protected:
	CStage0StaticTileMap0Object();
	CStage0StaticTileMap0Object(const CStage0StaticTileMap0Object& ref) = default;
	CStage0StaticTileMap0Object(CStage0StaticTileMap0Object&& ref) noexcept = default;

public:
	virtual ~CStage0StaticTileMap0Object() override = default;

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

class CStage0StaticTileMap1Object : public CStaticTileMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		bool mShowTutorial = false;
	};

protected:
	CStage0StaticTileMap1Object();
	CStage0StaticTileMap1Object(const CStage0StaticTileMap1Object& ref) = default;
	CStage0StaticTileMap1Object(CStage0StaticTileMap1Object&& ref) noexcept = default;

public:
	virtual ~CStage0StaticTileMap1Object() override = default;

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
	bool InitNPCs();

protected:
	void RollBackTutorial();
	void RemoveTutorial();

protected:
	std::weak_ptr<CBird> mTutorialBird;
	std::vector<std::weak_ptr<CColliderBox2D>> mTutirialTriggers;
};

class CStage0StaticTileMap2Object : public CStaticTileMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		bool mTalkOldlady = false;
	};

protected:
	CStage0StaticTileMap2Object();
	CStage0StaticTileMap2Object(const CStage0StaticTileMap2Object& ref) = default;
	CStage0StaticTileMap2Object(CStage0StaticTileMap2Object&& ref) noexcept = default;

public:
	virtual ~CStage0StaticTileMap2Object() override = default;

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
	bool InitNPCs();

protected:
	void RemoveTutorial();

protected:
	std::weak_ptr<COldLady> mTutorialOldLady;
	std::vector<std::weak_ptr<CColliderBox2D>> mTutirialTriggers;
};

class CStage0StaticTileMap3Object : public CStaticTileMapObject
{
	friend class CWorld;
	friend class CObject;

private:
	static constexpr int32 mOneBridgeBlockCount = 11;
	static constexpr int32 mAllBridgeBlockCount = 85;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		std::array<CTriggerOneTimeDropMapObject::FMapRuntimeData, mAllBridgeBlockCount> mBridgeBlockDatas;
	};

protected:
	CStage0StaticTileMap3Object();
	CStage0StaticTileMap3Object(const CStage0StaticTileMap3Object& ref) = default;
	CStage0StaticTileMap3Object(CStage0StaticTileMap3Object&& ref) noexcept = default;

public:
	virtual ~CStage0StaticTileMap3Object() override = default;

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
	bool InitNPCs();

protected:
	std::array<std::weak_ptr<CTriggerOneTimeDropMapObject>, mAllBridgeBlockCount> mBridgeBlocks;

	std::weak_ptr<CBird> mTutorialBird;
	std::weak_ptr<CColliderBox2D> mBGMTrigger;
	std::weak_ptr<CWorldTimeChangableBoxCollider> mSlowTrigger;
	std::weak_ptr<CColliderBox2D> mTutirialTrigger;
};
