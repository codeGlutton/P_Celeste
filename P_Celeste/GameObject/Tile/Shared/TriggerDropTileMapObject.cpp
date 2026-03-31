#include "Pch.h"
#include "TriggerDropTileMapObject.h"

#include "Component/ColliderBox2D.h"

bool CTriggerDropTileMapObject::Init()
{
	CDropTileMapObject::Init();

	mTriggerBox = CreateComponent<CColliderBox2D>("Trigger").lock();

	mTriggerBox->SetCollisionProfile("Item");
#ifdef _DEBUG

	mTriggerBox->SetDebugDraw(true);

#endif // DEBUG
	mTriggerBox->SetInheritScale(false);
	mTriggerBox->SetCollisionBeginFunction(this, &CTriggerDropTileMapObject::OnStartTriggerOverlap);
	mTriggerBox->SetGravitySacle(mDropGravityScale);

	return true;
}

void CTriggerDropTileMapObject::OnStartTriggerOverlap(const FVector3& HitPoint, CCollider* Collision)
{
	StartDrop();
}

std::shared_ptr<CTriggerDropTileMapObject> CTriggerDropTileMapObject::MakeQuikly(std::shared_ptr<CWorld> World, const FTriggerDropTileObjectInitInfo& InitInfo)
{
	std::shared_ptr<CTriggerDropTileMapObject> TriggerTileMap = CDynamicTileMapObject::MakeQuikly<CTriggerDropTileMapObject>(World, InitInfo);
	MakeQuikly(TriggerTileMap, InitInfo);

	return TriggerTileMap;
}

void CTriggerDropTileMapObject::MakeQuikly(std::shared_ptr<CTriggerDropTileMapObject> TirggerTileMapObject, const FTriggerDropTileObjectInitInfo& InitInfo)
{
	CDropTileMapObject::MakeQuikly(TirggerTileMapObject, InitInfo);

	TirggerTileMapObject->mTriggerBox->SetWorldPos(InitInfo.mTriggerBoxWorldPos + InitInfo.mTriggerBoxSize / 2.f);
	TirggerTileMapObject->mTriggerBox->SetBoxSize(InitInfo.mTriggerBoxSize);
}
