#include "ColliderOneWayBox2D.h"

#include "../Object/GameObject.h"

CColliderOneWayBox2D::CColliderOneWayBox2D()
{
	SetClassType<CColliderOneWayBox2D>();
	mColliderType = EColliderType::Box2D;
}

CColliderOneWayBox2D::CColliderOneWayBox2D(const CColliderOneWayBox2D& ref) :
	CColliderBox2D(ref)
{
}

CColliderOneWayBox2D::CColliderOneWayBox2D(CColliderOneWayBox2D&& ref) noexcept :
	CColliderBox2D(std::move(ref))
{
}

CColliderOneWayBox2D::~CColliderOneWayBox2D()
{
}

bool CColliderOneWayBox2D::PreCollision(FVector3& HitPoint, std::shared_ptr<CCollider> Dest)
{
	std::shared_ptr<CGameObject> DestGameObj = Dest->GetOwner().lock();
	if (DestGameObj == nullptr)
	{
		return false;
	}

	const FVector3& DestVelocity = DestGameObj->GetVelocity();
	if (DestVelocity.LengthSquared() <= mPassVelocityThreshold)
	{
		return true;
	}

	const float AxisCheck = FVector3::Normalize(DestVelocity).Dot(mOneWayAxis);
	if (AxisCheck <= -mPassDotThreshold)
	{
		return true;
	}
	return false;
}

