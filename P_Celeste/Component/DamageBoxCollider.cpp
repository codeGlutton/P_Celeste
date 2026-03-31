#include "Pch.h"
#include "DamageBoxCollider.h"

#include "Object/GameObject.h"

CDamageBoxCollider::CDamageBoxCollider()
{
	SetClassType<CDamageBoxCollider>();
}

CDamageBoxCollider* CDamageBoxCollider::Clone() const
{
	return new CDamageBoxCollider(*this);
}

bool CDamageBoxCollider::Init()
{
	CColliderBox2D::Init();

	SetCollisionProfile("Blocker");
	SetCollisionHitFunction([](const FVector3& HitPos, CCollider* Collider) {
		std::shared_ptr<CGameObject> GameObject = Collider->GetOwner().lock();
		if (GameObject != nullptr)
		{
			GameObject->TakeDamage(-1.f);
		}
		});
#ifdef _DEBUG

	SetDebugDraw(true);

#endif // DEBUG
	SetInheritScale(false);
	SetStatic(true);

    return true;
}
