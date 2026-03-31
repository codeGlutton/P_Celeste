#include "Pch.h"
#include "OneTimeDropTileMapObject.h"

#include "Component/ColliderBox2D.h"
#include "CollisionInfoManager.h"

void COneTimeDropTileMapObject::LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData)
{
	CDropTileMapObjectBase::LoadMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<COneTimeDropTileMapObject::FMapRuntimeData&>(RuntimeData);
    if (CurRuntimeData.mIsDropBlock == true)
    {
        SetEnable(false);
        SetAllTileLayerColor(FVector4(1.f, 1.f, 1.f, 0.f));
    }
    else
    {
        mAccLifeTime = 0.f;
        SetEnable(true);
        SetAllTileLayerColor(FVector4::White);
    }
}

void COneTimeDropTileMapObject::SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const
{
	CDropTileMapObjectBase::SaveMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<COneTimeDropTileMapObject::FMapRuntimeData&>(RuntimeData);
    CurRuntimeData.mIsDropBlock = (mDropState != EDropTileState::Idle);
}

void COneTimeDropTileMapObject::Update(float DeltaTime)
{
    CDropTileMapObjectBase::Update(DeltaTime);

    if (mDropState == EDropTileState::Drop)
    {
        mAccLifeTime += DeltaTime;
        if (mAccLifeTime >= mLifeTime)
        {
            mAccLifeTime = mLifeTime;

            mDropState = EDropTileState::Stop;
            std::shared_ptr<CSceneComponent> RootComp = GetRootComponent().lock();
            RootComp->SetSimulatePhysics(false);
            RootComp->ClearPhysics();
            RootComp->SetUseGravity(false);
            SetEnable(false);
        }

        FVector4 Color = FVector4::White;
        Color.w = 1.f - mAccLifeTime / mLifeTime;
        SetAllTileLayerColor(Color);
    }
}

std::shared_ptr<COneTimeDropTileMapObject> COneTimeDropTileMapObject::MakeQuikly(std::shared_ptr<CWorld> World, const FOneTimeDropTileObjectInitInfo& InitInfo)
{
	std::shared_ptr<COneTimeDropTileMapObject> DropTileMap = CDynamicTileMapObject::MakeQuikly<COneTimeDropTileMapObject>(World, InitInfo);
	MakeQuikly(DropTileMap, InitInfo);

	return DropTileMap;
}

void COneTimeDropTileMapObject::MakeQuikly(std::shared_ptr<COneTimeDropTileMapObject> DropTileMap, const FOneTimeDropTileObjectInitInfo& InitInfo)
{
	CDropTileMapObjectBase::MakeQuikly(DropTileMap, InitInfo);

	DropTileMap->mLifeTime = InitInfo.mLifeTime;
}