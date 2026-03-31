#pragma once

#include "Component/ColliderBox2D.h"

class CDamageBoxCollider : public CColliderBox2D
{
	friend class CGameObject;
	friend class CObject;

protected:
	CDamageBoxCollider();
	CDamageBoxCollider(const CDamageBoxCollider& ref) = default;
	CDamageBoxCollider(CDamageBoxCollider&& ref) noexcept = default;

public:
	virtual ~CDamageBoxCollider() override = default;

protected:
	virtual CDamageBoxCollider* Clone()	const override;

public:
	virtual bool Init() override;
};

