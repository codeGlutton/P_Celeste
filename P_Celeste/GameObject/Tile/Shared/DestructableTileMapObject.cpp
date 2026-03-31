#include "Pch.h"
#include "DestructableTileMapObject.h"

#include "Component/ColliderBox2D.h"
#include "Component/TileMapComponent.h"
#include "Component/TileMapRender.h"
#include "CollisionInfoManager.h"

bool CDestructableTileMapObject::Init()
{
    CDynamicTileMapObject::Init();
    return true;
}

void CDestructableTileMapObject::Update(float DeltaTime)
{
    CDynamicTileMapObject::Update(DeltaTime);

    if (mDestructState == EDestructableTileState::Destruct)
    {
        mFadeOutAccTime += DeltaTime;
        if (mFadeOutAccTime > mFadeOutMaxTime)
        {
            HideAll();
            return;
        }

        FVector4 CurFadeColor = FVector4(1 - mFadeRate, 1 - mFadeRate, 1 - mFadeRate, 1.f);
        CurFadeColor.w = MathUtils::Lerp(
            1.f, 0.f, mFadeOutAccTime / mFadeOutMaxTime
        );

        float CurRot = DeltaTime * MathUtils::Lerp(
            0.f, 90.f, mFadeOutAccTime / mFadeOutMaxTime
        );

        auto& LayerMap = GetAllTileLayers();
        for (auto& LayerPair : LayerMap)
        {
            std::shared_ptr<CTileMapComponent> MapComp = LayerPair.second.mTileMapComponent.lock();
            if (MapComp != nullptr)
            {
                int32 TileCount = MapComp->GetTileCountX() * MapComp->GetTileCountY();
                for (int32 i = 0; i < TileCount; ++i)
                {
                    std::shared_ptr<CTile> Tile = MapComp->GetTile(i).lock();
                    if (Tile != nullptr)
                    {
                        FVector2 NextPos = 
                            Tile->GetPos() + 
                            FVector2(0.f, GRAVITY2D * DeltaTime) +
                            mDestructResults[i].mDestructVelocity * DeltaTime;

                        Tile->SetOutLineColor(CurFadeColor);
                        Tile->SetPos(NextPos);
                        if (mDestructResults[i].mIsCountClockRotZ == true)
                        {
                            Tile->SetRotZ(Tile->GetRotZ() + DeltaTime * 90.f);
                        }
                        else
                        {
                            Tile->SetRotZ(Tile->GetRotZ() - DeltaTime * 90.f);
                        }
                    }
                }
            }
        }
    }
}

void CDestructableTileMapObject::LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData)
{
    CDynamicTileMapObject::LoadMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<CDestructableTileMapObject::FMapRuntimeData&>(RuntimeData);
    if (CurRuntimeData.mIsDestruct == true)
    {
        HideAll();
    }
}

void CDestructableTileMapObject::SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const
{
    CDynamicTileMapObject::SaveMapRuntimeData(RuntimeData);

    auto& CurRuntimeData = static_cast<CDestructableTileMapObject::FMapRuntimeData&>(RuntimeData);
    CurRuntimeData.mIsDestruct = (mDestructState != EDestructableTileState::Idle);
}

std::shared_ptr<CCollider> CDestructableTileMapObject::MakeMainCollider()
{
    std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Wall").lock();

    WallBoxComp->SetCollisionProfile("Dynamic");
#ifdef _DEBUG

    WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
    WallBoxComp->SetInheritScale(false);

    return WallBoxComp;
}

void CDestructableTileMapObject::OnDashed(std::shared_ptr<CGameObject> Interator)
{
    CDynamicTileMapObject::OnDashed(Interator);

    mDestructState = EDestructableTileState::Destruct;
    mFadeOutAccTime = 0.f;

    auto& LayerMap = GetAllTileLayers();
    for (auto& LayerPair : LayerMap)
    {
        std::shared_ptr<CTileMapComponent> MapComp = LayerPair.second.mTileMapComponent.lock();
        if (MapComp != nullptr)
        {
            MapComp->SetEnable(LayerPair.first == mDestructLayerName);
        }
    }
    mBodyComponent->SetEnable(false);

    std::shared_ptr<CTileMapComponent> DestructMapComp = GetTileMapComponent(mDestructLayerName);
    std::shared_ptr<CTileMapRender> DestructRenderComp = GetTileRenderComponent(mDestructLayerName);
    if (DestructMapComp != nullptr)
    {
        const FVector3 CenterPos = FVector3(DestructMapComp->GetMapSize() * 0.5f) + DestructRenderComp->GetWorldPos();
        const FVector3 DashDir = FVector3::Normalize(CenterPos - Interator->GetWorldPos());

        int32 TileCount = DestructMapComp->GetTileCountX() * DestructMapComp->GetTileCountY();
        mDestructResults.resize(TileCount);
        for (int32 i = 0; i < TileCount; ++i)
        {
            std::shared_ptr<CTile> Tile = DestructMapComp->GetTile(i).lock();
            if (Tile != nullptr)
            {
                FVector3 DestructDir = FVector3(Tile->GetPos()) - Interator->GetWorldPos();
                DestructDir.Normalize();

                mDestructResults[i].mIsCountClockRotZ = DashDir.Cross(DestructDir).z > 0.f;
                mDestructResults[i].mDestructVelocity = { mLaunchVelocity * DestructDir.x, mLaunchVelocity * DestructDir.y };
            }
        }
    }
}

void CDestructableTileMapObject::HideAll()
{
    mDestructState = EDestructableTileState::Hide;
    mRenderRootComponent->SetEnable(false);
    mBodyComponent->SetEnable(false);
}

std::shared_ptr<CDestructableTileMapObject> CDestructableTileMapObject::MakeQuikly(std::shared_ptr<CWorld> World, const FDestructableTileObjectInitInfo& InitInfo)
{
    std::shared_ptr<CDestructableTileMapObject> DestructableTileMap = CDynamicTileMapObject::MakeQuikly<CDestructableTileMapObject>(World, InitInfo);
    MakeQuikly(DestructableTileMap, InitInfo);

    return DestructableTileMap;
}

void CDestructableTileMapObject::MakeQuikly(std::shared_ptr<CDestructableTileMapObject> DestructableTileMap, const FDestructableTileObjectInitInfo& InitInfo)
{
    CDynamicTileMapObject::MakeQuikly(DestructableTileMap, InitInfo);

    std::shared_ptr<CColliderBox2D> BoxCollisionComp = std::static_pointer_cast<CColliderBox2D>(DestructableTileMap->mBodyComponent);
    if (BoxCollisionComp != nullptr)
    {
        BoxCollisionComp->SetRelativePos(InitInfo.mCollisionSize / 2.f);
        BoxCollisionComp->SetBoxSize(InitInfo.mCollisionSize);
    }

    DestructableTileMap->SetDestructLayerName(InitInfo.mDestructLayerName);
    DestructableTileMap->SetLaunchVelocity(InitInfo.mLaunchVelocity);
    DestructableTileMap->SetFadeOutMaxTime(InitInfo.mFadeOutMaxTime);
    DestructableTileMap->SetFadeRate(InitInfo.mFadeRate);
}