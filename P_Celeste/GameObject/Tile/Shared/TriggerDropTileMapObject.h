#pragma once

#include "GameObject/Tile/Shared/DropTileMapObject.h"

class CColliderBox2D;

struct FTriggerDropTileObjectInitInfo : public FDropTileObjectInitInfo
{
	FVector3 mTriggerBoxWorldPos;
	FVector2 mTriggerBoxSize;
};

class CTriggerDropTileMapObject : public CDropTileMapObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CTriggerDropTileMapObject() = default;
	CTriggerDropTileMapObject(const CTriggerDropTileMapObject& ref) = default;
	CTriggerDropTileMapObject(CTriggerDropTileMapObject&& ref) noexcept = default;

public:
	virtual ~CTriggerDropTileMapObject() override = default;

public:
	virtual bool Init() override;

private:
	virtual void OnStartTriggerOverlap(const FVector3& HitPoint, CCollider* Collision);

public:
	static std::shared_ptr<CTriggerDropTileMapObject> MakeQuikly(std::shared_ptr<CWorld> World, const FTriggerDropTileObjectInitInfo& InitInfo);
	static void MakeQuikly(std::shared_ptr<CTriggerDropTileMapObject> TirggerTileMapObject, const FTriggerDropTileObjectInitInfo& InitInfo);

protected:
	std::shared_ptr<CColliderBox2D> mTriggerBox;
};

