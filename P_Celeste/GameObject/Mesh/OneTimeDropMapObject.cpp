#include "Pch.h"
#include "OneTimeDropMapObject.h"

#include "Component/ColliderBox2D.h"
#include "Component/MeshComponent.h"
#include "CollisionInfoManager.h"

void COneTimeDropMapObject::LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData)
{
    CDropMapObjectBase::LoadMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<COneTimeDropMapObject::FMapRuntimeData&>(RuntimeData);
    if (CurRuntimeData.mIsDropBlock == true)
    {
        SetEnable(false);
        mMeshComponent->SetMaterialBaseColor(0, FVector4(1.f, 1.f, 1.f, 0.f));
    }
    else
    {
        mAccLifeTime = 0.f;
        SetEnable(true);
        mMeshComponent->SetMaterialBaseColor(0, FVector4::White);
    }
}

void COneTimeDropMapObject::SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const
{
    CDropMapObjectBase::SaveMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<COneTimeDropMapObject::FMapRuntimeData&>(RuntimeData);
    CurRuntimeData.mIsDropBlock = (mDropState != EDropState::Idle);
}

void COneTimeDropMapObject::Update(float DeltaTime)
{
    CDropMapObjectBase::Update(DeltaTime);

    if (mDropState == EDropState::Drop)
    {
        mAccLifeTime += DeltaTime;
        if (mAccLifeTime >= mLifeTime)
        {
            mAccLifeTime = mLifeTime;

            mDropState = EDropState::Stop;
            std::shared_ptr<CSceneComponent> RootComp = GetRootComponent().lock();
            RootComp->SetSimulatePhysics(false);
            RootComp->ClearPhysics();
            RootComp->SetUseGravity(false);
            SetEnable(false);
        }

        FVector4 Color = FVector4::White;
        Color.w = 1.f - mAccLifeTime / mLifeTime;
        mMeshComponent->SetMaterialBaseColor(0, Color);
    }
}

std::shared_ptr<COneTimeDropMapObject> COneTimeDropMapObject::MakeQuikly(std::shared_ptr<CWorld> World, const FOneTimeDropObjectInitInfo& InitInfo)
{
    std::shared_ptr<COneTimeDropMapObject> DropTileMap = CDynamicMapObject::MakeQuikly<COneTimeDropMapObject>(World, InitInfo);
    MakeQuikly(DropTileMap, InitInfo);

    return DropTileMap;
}

void COneTimeDropMapObject::MakeQuikly(std::shared_ptr<COneTimeDropMapObject> DropMap, const FOneTimeDropObjectInitInfo& InitInfo)
{
    CDropMapObjectBase::MakeQuikly(DropMap, InitInfo);

    DropMap->mLifeTime = InitInfo.mLifeTime;
}