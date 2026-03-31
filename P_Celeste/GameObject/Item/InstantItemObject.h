#pragma once

#include "GameObject/Item/ItemObject.h"

class CInstantItemObject : public CItemObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CInstantItemObject() :
		CItemObject(EItemObjectType::Instant)
	{
	}
	CInstantItemObject(const CInstantItemObject& ref) = default;
	CInstantItemObject(CInstantItemObject&& ref) noexcept = default;

public:
	virtual ~CInstantItemObject() override = default;

protected:
	virtual void OnBeginCollider(const FVector3& HitPos, CCollider* Other) override final;
};
