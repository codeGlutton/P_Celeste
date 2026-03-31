#pragma once

#include "GameObject/Item/ItemObject.h"

class CPlayer;

namespace ELazyItemObject
{
	enum Type : uint8
	{
		LandActive = 0,
		Count,
	};
}

class CLazyItemObject : public CItemObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CLazyItemObject() :
		CItemObject(EItemObjectType::Lazy)
	{
	}
	CLazyItemObject(const CLazyItemObject& ref) = default;
	CLazyItemObject(CLazyItemObject&& ref) noexcept = default;

public:
	virtual ~CLazyItemObject() override = default;

protected:
	virtual void OnBeginCollider(const FVector3& HitPos, CCollider* Other) override final;

public:
	virtual void OnFollowItem(std::shared_ptr<CPlayer> Player);
	virtual void OnReleaseItem(std::shared_ptr<CPlayer> Player);

public:
	ELazyItemObject::Type GetLazyType() const
	{
		return mLazyType;
	}

protected:
	ELazyItemObject::Type mLazyType = ELazyItemObject::LandActive;
};

