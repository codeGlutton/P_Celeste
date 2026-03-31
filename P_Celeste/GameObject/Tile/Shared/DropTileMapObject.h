#pragma once

#include "GameObject/Tile/Shared/DropTileMapObjectBase.h"

struct FDropTileObjectInitInfo : public FDropTileObjectBaseInitInfo
{
	std::string mHitSound;
};

class CDropTileMapObject : public CDropTileMapObjectBase
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public CDropTileMapObjectBase::FMapRuntimeData
	{
		FVector3 mDropPos = FVector3::Zero;
	};

protected:
	CDropTileMapObject() = default;
	CDropTileMapObject(const CDropTileMapObject& ref) = default;
	CDropTileMapObject(CDropTileMapObject&& ref) noexcept = default;

public:
	virtual ~CDropTileMapObject() override = default;

public:
	virtual void LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData) override;
	virtual void SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const override;

protected:
	virtual std::shared_ptr<CCollider> MakeMainCollider() override;

private:
	virtual void OnHitCollision(const FVector3& HitPoint, CCollider* Collision);
	
public:
	static std::shared_ptr<CDropTileMapObject> MakeQuikly(std::shared_ptr<CWorld> World, const FDropTileObjectInitInfo& InitInfo);

protected:
	static void MakeQuikly(std::shared_ptr<CDropTileMapObject> DropTileMap, const FDropTileObjectInitInfo& InitInfo);
};

