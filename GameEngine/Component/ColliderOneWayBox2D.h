#pragma once

#include "ColliderBox2D.h"

class CColliderOneWayBox2D : public CColliderBox2D
{
	friend class CGameObject;
	friend class CObject;

protected:
	CColliderOneWayBox2D();
	CColliderOneWayBox2D(const CColliderOneWayBox2D& ref);
	CColliderOneWayBox2D(CColliderOneWayBox2D&& ref)	noexcept;

public:
	virtual ~CColliderOneWayBox2D();

public:
	void SetOneWayAxis(const FVector3& Axis)
	{
		mOneWayAxis = Axis;
	}
	const FVector3& GetOneWayAxis() const
	{
		return mOneWayAxis;
	}

public:
	virtual bool PreCollision(FVector3& HitPoint, std::shared_ptr<CCollider> Dest) override;

protected:
	static constexpr float mPassVelocityThreshold = 0.0005f;
	static constexpr float mPassDotThreshold = -0.1f;
	FVector3 mOneWayAxis = FVector3::Axis[EAxis::Y];
};

