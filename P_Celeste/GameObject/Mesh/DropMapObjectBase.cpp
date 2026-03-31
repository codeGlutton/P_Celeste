#include "Pch.h"
#include "DropMapObjectBase.h"

#include "Component/ColliderBox2D.h"
#include "Component/MeshComponent.h"

#include "CollisionInfoManager.h"

const std::array<FVector2, 5> CDropMapObjectBase::mDropReadyShakeOffsets = {
    FVector2(3.f, -3.f),
    FVector2(-3.f, -1.5f),
    FVector2(1.5f, 0.f),
    FVector2(-1.5f, -1.5f),
    FVector2(0.f, 0.f)
};

void CDropMapObjectBase::LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData)
{
    CDynamicMapObject::LoadMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<CDropMapObjectBase::FMapRuntimeData&>(RuntimeData);
    if (CurRuntimeData.mIsDropBlock == true)
    {
        mDropState = EDropState::Stop;
    }
    else if (mDropState != EDropState::Idle)
    {
        SetWorldPos(mDropReadyPos);
        mDropState = EDropState::Idle;
        std::shared_ptr<CSceneComponent> RootComp = GetRootComponent().lock();
        RootComp->SetSimulatePhysics(false);
        RootComp->ClearPhysics();
        RootComp->SetUseGravity(false);
    }
}

void CDropMapObjectBase::SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const
{
    CDynamicMapObject::SaveMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<CDropMapObjectBase::FMapRuntimeData&>(RuntimeData);
    CurRuntimeData.mIsDropBlock = (mDropState != EDropState::Idle);
}

bool CDropMapObjectBase::Init()
{
    CDynamicMapObject::Init();

    return true;
}

void CDropMapObjectBase::Update(float DeltaTime)
{
    CDynamicMapObject::Update(DeltaTime);

    if (mDropState == EDropState::Ready)
    {
        mDropReadyShakeAccTime += DeltaTime;
        const float TickTime = mDropReadyMaxTime / mDropReadyShakeOffsets.size();
        mMeshComponent->SetWorldPos(
            MathUtils::Lerp(
                mMeshComponent->GetWorldPos(),
                mDropReadyMeshPos + mDropReadyShakeOffsets[mShakeIndex],
                mDropReadyShakeAccTime / TickTime
            )
        );
        while (mDropReadyShakeAccTime > TickTime)
        {
            mDropReadyShakeAccTime -= TickTime;
            ++mShakeIndex;
        }

        if (mShakeIndex >= mDropReadyShakeOffsets.size())
        {
            std::shared_ptr<CSceneComponent> RootComp = GetRootComponent().lock();

            mDropState = EDropState::Drop;
            mMeshComponent->SetWorldPos(mDropReadyMeshPos);
            RootComp->SetSimulatePhysics(true);
            RootComp->SetUseGravity(true);
        }
    }
}

std::shared_ptr<CCollider> CDropMapObjectBase::MakeMainCollider()
{
    std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Wall").lock();

    WallBoxComp->SetCollisionProfile("Dynamic");
#ifdef _DEBUG

    WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
    WallBoxComp->SetInheritScale(false);
    WallBoxComp->SetGravitySacle(mDropGravityScale);

    return WallBoxComp;
}

void CDropMapObjectBase::OnLanded(std::shared_ptr<CGameObject> Interator)
{
    CDynamicMapObject::OnLanded(Interator);
    StartDrop();
}

void CDropMapObjectBase::OnGraped(std::shared_ptr<CGameObject> Interator)
{
    CDynamicMapObject::OnGraped(Interator);
    StartDrop();
}

void CDropMapObjectBase::StartDrop()
{
    if (mDropState != EDropState::Idle)
    {
        return;
    }

    mDropState = EDropState::Ready;
    mDropReadyPos = GetWorldPos();
    mDropReadyMeshPos = mMeshComponent->GetWorldPos();
    mDropReadyShakeAccTime = 0.f;
    mShakeIndex = 0;
}

void CDropMapObjectBase::SetDropGravityScale(float GravityScale)
{
    mDropGravityScale = GravityScale;
    std::shared_ptr<CSceneComponent> RootComp = GetRootComponent().lock();
    RootComp->SetGravitySacle(mDropGravityScale);
}

void CDropMapObjectBase::MakeQuikly(std::shared_ptr<CDropMapObjectBase> DropMap, const FDropObjectBaseInitInfo& InitInfo)
{
    CDynamicMapObject::MakeQuikly(DropMap, InitInfo);

    std::shared_ptr<CColliderBox2D> BoxCollisionComp = std::static_pointer_cast<CColliderBox2D>(DropMap->mBodyComponent);
    if (BoxCollisionComp != nullptr)
    {
        BoxCollisionComp->SetRelativePos(InitInfo.mCollisionSize / 2.f);
        BoxCollisionComp->SetBoxSize(InitInfo.mCollisionSize);
    }

    DropMap->SetDropGravityScale(InitInfo.mDropGravityScale);
    DropMap->mDropReadyMaxTime = InitInfo.mDropReadyMaxTime;
}
