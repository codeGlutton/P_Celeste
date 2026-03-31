#include "Pch.h"
#include "DropTileMapObjectBase.h"

#include "Component/ColliderBox2D.h"
#include "Component/TileMapRender.h"
#include "CollisionInfoManager.h"

const std::array<FVector2, 5> CDropTileMapObjectBase::mDropReadyShakeOffsets = {
    FVector2(3.f, -3.f),
    FVector2(-3.f, -1.5f),
    FVector2(1.5f, 0.f),
    FVector2(-1.5f, -1.5f),
    FVector2(0.f, 0.f)
};

void CDropTileMapObjectBase::LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData)
{
    CDynamicTileMapObject::LoadMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<CDropTileMapObjectBase::FMapRuntimeData&>(RuntimeData);
    if (CurRuntimeData.mIsDropBlock == true)
    {
        mDropState = EDropTileState::Stop;
    }
    else if (mDropState != EDropTileState::Idle)
    {
        SetWorldPos(mDropReadyPos);
        mDropState = EDropTileState::Idle;
        std::shared_ptr<CSceneComponent> RootComp = GetRootComponent().lock();
        RootComp->SetSimulatePhysics(false);
        RootComp->ClearPhysics();
        RootComp->SetUseGravity(false);
    }
}

void CDropTileMapObjectBase::SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const
{
    CDynamicTileMapObject::SaveMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<CDropTileMapObjectBase::FMapRuntimeData&>(RuntimeData);
    CurRuntimeData.mIsDropBlock = (mDropState != EDropTileState::Idle);
}

bool CDropTileMapObjectBase::Init()
{
    CDynamicTileMapObject::Init();

    return true;
}

void CDropTileMapObjectBase::Update(float DeltaTime)
{
    CDynamicTileMapObject::Update(DeltaTime);

    if (mDropState == EDropTileState::Ready)
    {
        mDropReadyShakeAccTime += DeltaTime;
        const float TickTime = mDropReadyMaxTime / mDropReadyShakeOffsets.size();
        mRenderRootComponent->SetWorldPos(
            MathUtils::Lerp(
                mRenderRootComponent->GetWorldPos(),
                mDropReadyPos + mDropReadyShakeOffsets[mShakeIndex],
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

            mDropState = EDropTileState::Drop;
            mRenderRootComponent->SetWorldPos(mDropReadyPos);
            RootComp->SetSimulatePhysics(true);
            RootComp->SetUseGravity(true);

            mWorld.lock()->GetWorldAssetManager().lock()->Sound3DPlay(mDropSound, GetWorldPos());
        }
    }
}

std::shared_ptr<CCollider> CDropTileMapObjectBase::MakeMainCollider()
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

void CDropTileMapObjectBase::OnLanded(std::shared_ptr<CGameObject> Interator)
{
    CDynamicTileMapObject::OnLanded(Interator);
    StartDrop();
}

void CDropTileMapObjectBase::OnGraped(std::shared_ptr<CGameObject> Interator)
{
    CDynamicTileMapObject::OnGraped(Interator);
    StartDrop();
}

void CDropTileMapObjectBase::StartDrop()
{
    if (mDropState != EDropTileState::Idle)
    {
        return;
    }

    mDropState = EDropTileState::Ready;
    mDropReadyPos = GetWorldPos();
    mDropReadyShakeAccTime = 0.f;
    mShakeIndex = 0;

    mWorld.lock()->GetWorldAssetManager().lock()->Sound3DPlay(mShakeSound, GetWorldPos());
}

void CDropTileMapObjectBase::SetDropGravityScale(float GravityScale)
{
    mDropGravityScale = GravityScale;
    std::shared_ptr<CSceneComponent> RootComp = GetRootComponent().lock();
    RootComp->SetGravitySacle(mDropGravityScale);
}

void CDropTileMapObjectBase::MakeQuikly(std::shared_ptr<CDropTileMapObjectBase> DropTileMap, const FDropTileObjectBaseInitInfo& InitInfo)
{
    CDynamicTileMapObject::MakeQuikly(DropTileMap, InitInfo);

    std::shared_ptr<CColliderBox2D> BoxCollisionComp = std::static_pointer_cast<CColliderBox2D>(DropTileMap->mBodyComponent);
    if (BoxCollisionComp != nullptr)
    {
        BoxCollisionComp->SetRelativePos(InitInfo.mCollisionSize / 2.f);
        BoxCollisionComp->SetBoxSize(InitInfo.mCollisionSize);
    }

    DropTileMap->SetDropGravityScale(InitInfo.mDropGravityScale);
    DropTileMap->mDropReadyMaxTime = InitInfo.mDropReadyMaxTime;

    DropTileMap->mShakeSound = InitInfo.mShakeSound;
    DropTileMap->mDropSound = InitInfo.mDropSound;
}