#include "Pch.h"
#include "DynamicTileMapObject.h"

#include "Component/Collider.h"
#include "Component/TileMapRender.h"
#include "Component/TileMapComponent.h"

int32 CDynamicTileMapObject::mMapMaxIndex = 0;

CDynamicTileMapObject::CDynamicTileMapObject()
{
	mTileMapObjectType = ETileMapObjectType::Dynamic;
}

bool CDynamicTileMapObject::Init()
{
	CLayeredTileMapObject::Init();

	std::shared_ptr<CSceneComponent> RootComp = CreateComponent<CSceneComponent>("RootComp").lock();
	RootComp->SetWorldPos(0.f, 0.f);
	RootComp->SetWorldScale(1.f, 1.f);

	mBodyComponent = MakeMainCollider();
	if (mBodyComponent != nullptr)
	{
		mBodyComponent->SetStatic(false);
		mBodyComponent->SetInvMass(0.f);
	}

	mRenderRootComponent = CreateComponent<CSceneComponent>("RenderRootComp").lock();
	if (mRenderRootComponent != nullptr)
	{
		mRenderRootComponent->SetRelativePos(0.f, 0.f);
		mRenderRootComponent->SetRelativeScale(1.f, 1.f);
	}

	return true;
}

void CDynamicTileMapObject::LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData)
{
}

void CDynamicTileMapObject::SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const
{
}

void CDynamicTileMapObject::OnLanded(std::shared_ptr<CGameObject> Interator)
{
	AttachGameObject(Interator);
}

void CDynamicTileMapObject::OnJumped(std::shared_ptr<CGameObject> Interator)
{
	DetachGameObject(Interator);
}

void CDynamicTileMapObject::OnGraped(std::shared_ptr<CGameObject> Interator)
{
	AttachGameObject(Interator);
}

void CDynamicTileMapObject::OnReleased(std::shared_ptr<CGameObject> Interator)
{
	DetachGameObject(Interator);
}

void CDynamicTileMapObject::MakeQuikly(std::shared_ptr<CDynamicTileMapObject> TileMapObject, const FDynamicTileObjectInitInfo& InitInfo)
{
	TileMapObject->SetWorldPos(InitInfo.mWorldPos);

	for (auto& LayerInfo : InitInfo.mLayerInitInfos)
	{
		FTileMapLayer& MapLayer = TileMapObject->AddTileLayer(LayerInfo.mLayerName, "RenderRootComp");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp != nullptr)
		{
			if (LayerInfo.mIsEnableAlpha == true)
			{
				TileRenderComp->EnableTileAlphaBlend();
			}
			TileRenderComp->SetRenderLayer(LayerInfo.mRenderLayerName);
			TileRenderComp->SetRelativePos(0.f, 0.f);
			TileMapObject->LoadMap(LayerInfo.mLayerName, LayerInfo.mFilePath.c_str());
		}
	}
}