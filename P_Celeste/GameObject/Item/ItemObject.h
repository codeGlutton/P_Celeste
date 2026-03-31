#pragma once

#include "Object/GameObject.h"
#include "World/World.h"

class CCollider;
class CMeshComponent;
class CSpringArmComponent;

class CPlayer;

struct FItemObjectInitInfo
{
	FVector3 mWorldPos;
	FVector3 mMeshWorldPos;
	FVector3 mMeshScale;
	std::string mTextureName;
	std::string mRenderLayerName;
	bool mIsEnableAlpha = false;
};

enum class EItemObjectType
{
	Instant,
	Lazy,
};

class CItemObject : public CGameObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CItemObject() = default;
	CItemObject(EItemObjectType ItemObjectType) :
		mItemObjectType(ItemObjectType)
	{
	}
	CItemObject(const CItemObject& ref) = default;
	CItemObject(CItemObject&& ref) noexcept = default;

public:
	virtual ~CItemObject() override = default;

public:
	virtual bool Init() override;

protected:
	virtual std::shared_ptr<CCollider> MakeMainCollider() = 0;

public:
	virtual void OnActiveItem(std::shared_ptr<CPlayer> Player);

private:
	virtual void OnBeginCollider(const FVector3& HitPos, CCollider* Other) = 0;

public:
	bool IsItemActivated() const
	{
		return mIsItemActivated;
	}

protected:
	template<typename T>
	static std::shared_ptr<T> MakeQuikly(std::shared_ptr<CWorld> World, const FItemObjectInitInfo& InitInfo)
	{
		std::shared_ptr<CItemObject> ItemObject = World->CreateGameObject<T>(
			"ItemObj" + std::to_string(CItemObject::mItemMaxIndex)
		).lock();
		return std::static_pointer_cast<T>(ItemObject);
	}

protected:
	static void MakeQuikly(std::shared_ptr<CItemObject> ItemObj, const FItemObjectInitInfo& InitInfo);

protected:
	static int32 mItemMaxIndex;

protected:
	std::shared_ptr<CCollider> mBodyComponent;
	std::shared_ptr<CMeshComponent> mMeshComponent;
	std::shared_ptr<CSpringArmComponent> mSpringArmComponent;

protected:
	bool mIsItemActivated = false;

protected:
	const EItemObjectType mItemObjectType = EItemObjectType::Instant;
};

