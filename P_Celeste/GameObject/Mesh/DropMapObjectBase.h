#pragma once

#include "GameObject/Mesh/DynamicMapObject.h"
#include "GameObject/Tile/MapInfo.h"

enum class EDropState : uint8
{
	Idle,
	Ready,
	Drop,
	Stop
};

struct FDropObjectBaseInitInfo : public FDynamicObjectInitInfo
{
	FVector2 mCollisionSize;
	float mDropGravityScale = 2.f;
	float mDropReadyMaxTime = 0.5f;
};

class CDropMapObjectBase : public CDynamicMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		bool mIsDropBlock = false;
	};

protected:
	CDropMapObjectBase() = default;
	CDropMapObjectBase(const CDropMapObjectBase& ref) = default;
	CDropMapObjectBase(CDropMapObjectBase&& ref) noexcept = default;

public:
	virtual ~CDropMapObjectBase() override = default;

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
	static void MakeQuikly(std::shared_ptr<CDropMapObjectBase> DropMap, const FDropObjectBaseInitInfo& InitInfo);

protected:
	EDropState mDropState = EDropState::Idle;
	float mDropGravityScale = 2.f;

protected:
	float mDropReadyMaxTime = 0.5f;

protected:
	FVector3 mDropReadyPos;
	FVector3 mDropReadyMeshPos;
	static const std::array<FVector2, 5> mDropReadyShakeOffsets;
	int32 mShakeIndex = 0;
	float mDropReadyShakeAccTime = 0.f;
};

