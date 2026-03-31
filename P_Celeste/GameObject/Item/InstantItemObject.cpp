#include "Pch.h"
#include "InstantItemObject.h"

#include "Character/Player/Player.h"

#include "Component/Collider.h"

void CInstantItemObject::OnBeginCollider(const FVector3& HitPos, CCollider* Other)
{
	SetEnable(false);

	auto Player = std::static_pointer_cast<CPlayer>(Other->GetOwner().lock());
	if (Player != nullptr)
	{
		OnActiveItem(Player);
	}
}
