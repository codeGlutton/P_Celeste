#include "Pch.h"
#include "LayeredTileMapObject.h"

#include "Component/TileMapRender.h"
#include "Component/TileMapComponent.h"

void CLayeredTileMapObject::SetTileLayerBlendAlpha(const std::string& LayerName)
{
	FTileMapLayer& TileLayer = mTileLayers[LayerName];
	if (TileLayer.mTileRenderComponent.expired() == false)
	{
		std::shared_ptr<CTileMapRender> TileRenderComp = TileLayer.mTileRenderComponent.lock();
		TileRenderComp->EnableAlphaBlend();
	}
}

void CLayeredTileMapObject::SetAllTileLayerBlendAlpha()
{
	for (auto& TileLayerPair : mTileLayers)
	{
		if (TileLayerPair.second.mTileRenderComponent.expired() == false)
		{
			std::shared_ptr<CTileMapRender> TileRenderComp = TileLayerPair.second.mTileRenderComponent.lock();
			TileRenderComp->EnableAlphaBlend();
		}
	}
}

void CLayeredTileMapObject::SetTileLayerColor(const std::string& LayerName, const FVector4& Color)
{
	FTileMapLayer& TileLayer = mTileLayers[LayerName];
	if (TileLayer.mTileMapComponent.expired() == false)
	{
		std::shared_ptr<CTileMapComponent> TileMapComp = TileLayer.mTileMapComponent.lock();
		int32 IndexCount = TileMapComp->GetTileCountX() * TileMapComp->GetTileCountY();
		for (int32 i = 0; i < IndexCount; ++i)
		{
			TileMapComp->GetTile(i).lock()->SetOutLineColor(Color);
		}
	}
}

void CLayeredTileMapObject::SetAllTileLayerColor(const FVector4& Color)
{
	for (auto& TileLayerPair : mTileLayers)
	{
		if (TileLayerPair.second.mTileMapComponent.expired() == false)
		{
			std::shared_ptr<CTileMapComponent> TileMapComp = TileLayerPair.second.mTileMapComponent.lock();
			int32 IndexCount = TileMapComp->GetTileCountX() * TileMapComp->GetTileCountY();
			for (int32 i = 0; i < IndexCount; ++i)
			{
				TileMapComp->GetTile(i).lock()->SetOutLineColor(Color);
			}
		}
	}
}

FTileMapLayer& CLayeredTileMapObject::AddTileLayer(const std::string& LayerName, const std::string& ParentName)
{
	FTileMapLayer& TileLayer = mTileLayers[LayerName];
	if (TileLayer.mTileMapComponent.expired() == true)
	{
		TileLayer.mTileMapComponent = CreateComponent<CTileMapComponent>("TileMap", ParentName);
		TileLayer.mTileRenderComponent = CreateComponent<CTileMapRender>("TileRender", ParentName);

		std::shared_ptr<CTileMapRender> TileRenderComp = TileLayer.mTileRenderComponent.lock();
		TileRenderComp->SetTileMapComponent(TileLayer.mTileMapComponent);
		TileRenderComp->SetWorldScale(1.f, 1.f);
	}
	return TileLayer;
}

void CLayeredTileMapObject::RemoveTileLayer(const std::string& LayerName)
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	if (TileMapComp != nullptr)
	{
		TileMapComp->Destroy();
	}
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileRenderComp != nullptr)
	{
		TileRenderComp->Destroy();
	}

	mTileLayers.erase(LayerName);
}

void CLayeredTileMapObject::LoadMap(const std::string& LayerName, const TCHAR* FileName, const std::string& PathName, const std::string& ParentName)
{
	FTileMapLayer& TileLayer = AddTileLayer(LayerName, ParentName);

	std::shared_ptr<CTileMapComponent> TileMapComp = TileLayer.mTileMapComponent.lock();
	TileMapComp->Load(FileName, PathName);

	const FVector2& MapSize = TileMapComp->GetMapSize();
	if (mTileMapMaxSize.x < MapSize.x)
	{
		mTileMapMaxSize.x = MapSize.x;
	}
	if (mTileMapMaxSize.y < MapSize.y)
	{
		mTileMapMaxSize.y = MapSize.y;
	}
}
