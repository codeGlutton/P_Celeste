#pragma once

#include "GameObject/Interator/PlayerInteractableObject.h"

class CTileMapRender;
class CTileMapComponent;

enum class ETileMapObjectType
{
	Static,
	Dynamic
};

struct FTileMapLayer
{
	std::weak_ptr<CTileMapRender> mTileRenderComponent;
	std::weak_ptr<CTileMapComponent> mTileMapComponent;
};

class CLayeredTileMapObject : public CPlayerInteractableObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CLayeredTileMapObject() = default;
	CLayeredTileMapObject(const CLayeredTileMapObject& ref) = default;
	CLayeredTileMapObject(CLayeredTileMapObject&& ref) noexcept = default;

public:
	virtual ~CLayeredTileMapObject() override = default;

public:
	const FVector2& GetTileMapMaxSize() const
	{
		return mTileMapMaxSize;
	}

public:
	void SetTileLayerBlendAlpha(const std::string& LayerName);
	void SetAllTileLayerBlendAlpha();

	void SetTileLayerColor(const std::string& LayerName, const FVector4& Color);
	void SetAllTileLayerColor(const FVector4& Color);

protected:
	FTileMapLayer& AddTileLayer(const std::string& LayerName, const std::string& ParentName = "Root");
	void RemoveTileLayer(const std::string& LayerName);

	FTileMapLayer& GetTileLayer(const std::string& LayerName)
	{
		return mTileLayers.at(LayerName);
	}
	const FTileMapLayer& GetTileLayer(const std::string& LayerName) const
	{
		return mTileLayers.at(LayerName);
	}

	std::unordered_map<std::string, FTileMapLayer>& GetAllTileLayers()
	{
		return mTileLayers;
	}
	const std::unordered_map<std::string, FTileMapLayer>& GetAllTileLayers() const
	{
		return mTileLayers;
	}

	std::shared_ptr<CTileMapRender> GetTileRenderComponent(const std::string& LayerName) const
	{
		if (mTileLayers.find(LayerName) == mTileLayers.end())
		{
			return nullptr;
		}
		return mTileLayers.at(LayerName).mTileRenderComponent.lock();
	}

	std::shared_ptr<CTileMapComponent> GetTileMapComponent(const std::string& LayerName) const
	{
		if (mTileLayers.find(LayerName) == mTileLayers.end())
		{
			return nullptr;
		}
		return mTileLayers.at(LayerName).mTileMapComponent.lock();
	}

public:
	void LoadMap(const std::string& LayerName, const TCHAR* FileName, const std::string& PathName = "TileMap", const std::string& ParentName = "Root");

protected:
	ETileMapObjectType mTileMapObjectType = ETileMapObjectType::Static;

private:
	std::unordered_map<std::string, FTileMapLayer> mTileLayers;
	FVector2 mTileMapMaxSize;
};

