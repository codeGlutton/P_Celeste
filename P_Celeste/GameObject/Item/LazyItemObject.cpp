#include "Pch.h"
#include "LazyItemObject.h"

#include "Character/Player/Player.h"

#include "Component/Collider.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/PlayerGameState.h"

void CLazyItemObject::OnBeginCollider(const FVector3& HitPos, CCollider* Other)
{
	auto Player = std::static_pointer_cast<CPlayer>(Other->GetOwner().lock());
	if (Player != nullptr && Player->GetMovementComp()->IsActiveMovement("Dead") == false)
	{
		Player->GetMovementComp()->GetSharedState<FPlayerGameState>().AddLazyItems(std::static_pointer_cast<CLazyItemObject>(mSelf.lock()));
	}
}

void CLazyItemObject::OnFollowItem(std::shared_ptr<CPlayer> Player)
{
	mBodyComponent->SetEnable(false);
}

void CLazyItemObject::OnReleaseItem(std::shared_ptr<CPlayer> Player)
{
	mBodyComponent->SetEnable(true);
}
