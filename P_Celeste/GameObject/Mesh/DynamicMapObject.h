#pragma once

#include "GameObject/Interator/PlayerInteractableObject.h"
#include "World/World.h"

class CWorld;

class CCollider;
class CMeshComponent;

struct FMapRuntimeDataBase;

struct FDynamicObjectInitInfo
{
	FVector3 mWorldPos;
	FVector3 mMeshWorldPos;
	FVector3 mMeshScale;
	std::string mTextureName;
	std::string mRenderLayerName;
	bool mIsEnableAlpha = false;
};

class CDynamicMapObject : public CPlayerInteractableObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CDynamicMapObject() = default;
	CDynamicMapObject(const CDynamicMapObject& ref) = default;
	CDynamicMapObject(CDynamicMapObject&& ref) noexcept = default;

public:
	virtual ~CDynamicMapObject() override = default;

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
	static std::shared_ptr<T> MakeQuikly(std::shared_ptr<CWorld> World, const FDynamicObjectInitInfo& InitInfo)
	{
		std::shared_ptr<CDynamicMapObject> MapObject = World->CreateGameObject<T>(
			"DynamicMap" + std::to_string(CDynamicMapObject::mMapMaxIndex)
		).lock();
		return std::static_pointer_cast<T>(MapObject);
	}
	static void MakeQuikly(std::shared_ptr<CDynamicMapObject> MapObject, const FDynamicObjectInitInfo& InitInfo);

protected:
	static int32 mMapMaxIndex;

protected:
	std::shared_ptr<CCollider> mBodyComponent;
	std::shared_ptr<CMeshComponent> mMeshComponent;
};

