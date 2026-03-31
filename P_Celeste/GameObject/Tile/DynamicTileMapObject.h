#pragma once

#include "GameObject/Tile/LayeredTileMapObject.h"
#include "World/World.h"

class CWorld;
class CTileMapRender;

struct FMapRuntimeDataBase;

struct FDynamicTileLayerInitInfo
{
	std::wstring mFilePath;
	std::string mLayerName;
	std::string mRenderLayerName;
	bool mIsEnableAlpha = false;
};

struct FDynamicTileObjectInitInfo
{
	FVector3 mWorldPos;
	std::vector<FDynamicTileLayerInitInfo> mLayerInitInfos;
};

class CCollider;

class CDynamicTileMapObject : public CLayeredTileMapObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CDynamicTileMapObject();
	CDynamicTileMapObject(const CDynamicTileMapObject& ref) = default;
	CDynamicTileMapObject(CDynamicTileMapObject&& ref) noexcept = default;

public:
	virtual ~CDynamicTileMapObject() override = default;

public:
	virtual bool Init() override;

public:
	virtual void LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData);
	virtual void SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const;

public:
	virtual void OnLanded(std::shared_ptr<CGameObject> Interator) override;
	virtual void OnJumped(std::shared_ptr<CGameObject> Interator) override;
	virtual void OnGraped(std::shared_ptr<CGameObject> Interator) override;
	virtual void OnReleased(std::shared_ptr<CGameObject> Interator) override;

private:
	virtual std::shared_ptr<CCollider> MakeMainCollider() = 0;

protected:
	template<typename T>
	static std::shared_ptr<T> MakeQuikly(std::shared_ptr<CWorld> World, const FDynamicTileObjectInitInfo& InitInfo)
	{
		std::shared_ptr<CDynamicTileMapObject> TileMapObject = World->CreateGameObject<T>(
			"DynamicTileMap" + std::to_string(CDynamicTileMapObject::mMapMaxIndex)
		).lock();
		return std::static_pointer_cast<T>(TileMapObject);
	}
	static void MakeQuikly(std::shared_ptr<CDynamicTileMapObject> TileMapObject, const FDynamicTileObjectInitInfo& InitInfo);

protected:
	static int32 mMapMaxIndex;

protected:
	std::shared_ptr<CCollider> mBodyComponent;
	std::shared_ptr<CSceneComponent> mRenderRootComponent;
};

