#include "Pch.h"
#include "DropTileMapObject.h"

#include "Component/ColliderBox2D.h"
#include "CollisionInfoManager.h"

void CDropTileMapObject::LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData)
{
    CDropTileMapObjectBase::LoadMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<CDropTileMapObject::FMapRuntimeData&>(RuntimeData);
    if (CurRuntimeData.mIsDropBlock == true)
    {
        SetWorldPos(CurRuntimeData.mDropPos);
    }
}

void CDropTileMapObject::SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const
{
    CDropTileMapObjectBase::SaveMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<CDropTileMapObject::FMapRuntimeData&>(RuntimeData);
    CurRuntimeData.mIsDropBlock = (mDropState != EDropTileState::Idle);
    CurRuntimeData.mDropPos = GetWorldPos();
}

std::shared_ptr<CCollider> CDropTileMapObject::MakeMainCollider()
{
    auto WallBoxComp = std::static_pointer_cast<CColliderBox2D>(CDropTileMapObjectBase::MakeMainCollider());

    WallBoxComp->SetCollisionHitFunction(this, &CDropTileMapObject::OnHitCollision);

    return WallBoxComp;
}

void CDropTileMapObject::OnHitCollision(const FVector3& HitPoint, CCollider* Collision)
{
    if (Collision->GetCollisionProfile() == CCollisionInfoManager::GetInst()->FindProfile("Player"))
    {
        return;
    }

    if (mDropState == EDropTileState::Drop)
    {
        FVector3 HitNormal = HitPoint - mBodyComponent->GetWorldPos();
        HitNormal.Normalize();
        if (HitNormal.Dot(-FVector3::Axis[EAxis::Y]) > 0.f)
        {
            mDropState = EDropTileState::Stop;
            std::shared_ptr<CSceneComponent> RootComp = GetRootComponent().lock();
            RootComp->SetSimulatePhysics(false);
            RootComp->ClearPhysics();
            RootComp->SetUseGravity(false);

            mWorld.lock()->GetWorldAssetManager().lock()->Sound3DPlay(mHitSound, GetWorldPos());
        }
    }
}

std::shared_ptr<CDropTileMapObject> CDropTileMapObject::MakeQuikly(std::shared_ptr<CWorld> World, const FDropTileObjectInitInfo& InitInfo)
{
    std::shared_ptr<CDropTileMapObject> DropTileMap = CDynamicTileMapObject::MakeQuikly<CDropTileMapObject>(World, InitInfo);
    MakeQuikly(DropTileMap, InitInfo);

    return DropTileMap;
}

void CDropTileMapObject::MakeQuikly(std::shared_ptr<CDropTileMapObject> DropTileMap, const FDropTileObjectInitInfo& InitInfo)
{
    CDropTileMapObjectBase::MakeQuikly(DropTileMap, InitInfo);

    DropTileMap->mHitSound = InitInfo.mHitSound;
}
