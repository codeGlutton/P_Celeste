#pragma once

#include "GameObject/Tile/DynamicTileMapObject.h"
#include "GameObject/Tile/MapInfo.h"

enum class EDestructableTileState : uint8
{
	Idle,
	Destruct,
	Hide,
};

struct FDestructableTileObjectInitInfo : public FDynamicTileObjectInitInfo
{
	FVector2 mCollisionSize;
	float mLaunchVelocity = 500.f;
	float mFadeOutMaxTime = 0.5f;
	float mFadeRate = 0.2f;
	std::string mDestructLayerName;
};

struct FDestructableResult
{
	FVector2 mDestructVelocity;
	bool mIsCountClockRotZ;
};

class CDestructableTileMapObject : public CDynamicTileMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
		bool mIsDestruct = false;
	};

protected:
	CDestructableTileMapObject() = default;
	CDestructableTileMapObject(const CDestructableTileMapObject& ref) = default;
	CDestructableTileMapObject(CDestructableTileMapObject&& ref) noexcept = default;

public:
	virtual ~CDestructableTileMapObject() override = default;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

public:
	virtual void LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData) override;
	virtual void SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const override;

private:
	virtual std::shared_ptr<CCollider> MakeMainCollider() override;

public:
	virtual void OnDashed(std::shared_ptr<CGameObject> Interator) override;

protected:
	void HideAll();

public:
	void SetLaunchVelocity(float Velocity)
	{
		mLaunchVelocity = Velocity;
	}
	void SetFadeRate(float Rate)
	{
		mFadeRate = Rate;
	}
	void SetFadeOutMaxTime(float MaxTime)
	{
		mFadeOutMaxTime = MaxTime;
	}
	void SetDestructLayerName(const std::string& LayerName)
	{
		mDestructLayerName = LayerName;
	}

public:
	static std::shared_ptr<CDestructableTileMapObject> MakeQuikly(std::shared_ptr<CWorld> World, const FDestructableTileObjectInitInfo& InitInfo);

protected:
	static void MakeQuikly(std::shared_ptr<CDestructableTileMapObject> DestructableTileMap, const FDestructableTileObjectInitInfo& InitInfo);

protected:
	EDestructableTileState mDestructState = EDestructableTileState::Idle;
	std::vector<FDestructableResult> mDestructResults;
	float mFadeOutAccTime = 0.f;

protected:
	float mLaunchVelocity = 500.f;
	float mFadeOutMaxTime = 0.5f;
	float mFadeRate = 0.2f;
	std::string mDestructLayerName;
};

