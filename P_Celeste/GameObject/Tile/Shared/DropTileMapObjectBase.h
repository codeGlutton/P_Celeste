#pragma once

#include "GameObject/Tile/DynamicTileMapObject.h"
#include "GameObject/Tile/MapInfo.h"

enum class EDropTileState : uint8
{
	Idle,
	Ready,
	Drop,
	Stop
};

struct FDropTileObjectBaseInitInfo : public FDynamicTileObjectInitInfo
{
	FVector2 mCollisionSize;
	float mDropGravityScale = 2.f;
	float mDropReadyMaxTime = 0.5f;

	std::string mShakeSound;
	std::string mDropSound;
};

class CDropTileMapObjectBase : public CDynamicTileMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		bool mIsDropBlock = false;
	};

protected:
	CDropTileMapObjectBase() = default;
	CDropTileMapObjectBase(const CDropTileMapObjectBase& ref) = default;
	CDropTileMapObjectBase(CDropTileMapObjectBase&& ref) noexcept = default;

public:
	virtual ~CDropTileMapObjectBase() override = default;

public:
	virtual void LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData) override;
	virtual void SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const override;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

protected:
	virtual std::shared_ptr<CCollider> MakeMainCollider() override;

public:
	virtual void OnLanded(std::shared_ptr<CGameObject> Interator) override;
	virtual void OnGraped(std::shared_ptr<CGameObject> Interator) override;

protected:
	virtual void StartDrop();

public:
	void SetDropGravityScale(float GravityScale);

protected:
	static void MakeQuikly(std::shared_ptr<CDropTileMapObjectBase> DropTileMap, const FDropTileObjectBaseInitInfo& InitInfo);

protected:
	EDropTileState mDropState = EDropTileState::Idle;
	float mDropGravityScale = 2.f;

protected:
	float mDropReadyMaxTime = 0.5f;

protected:
	FVector3 mDropReadyPos;
	static const std::array<FVector2, 5> mDropReadyShakeOffsets;
	int32 mShakeIndex = 0;
	float mDropReadyShakeAccTime = 0.f;

	std::string mShakeSound;
	std::string mDropSound;
	std::string mHitSound;
};

