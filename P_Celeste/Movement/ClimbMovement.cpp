#include "Pch.h"
#include "ClimbMovement.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/PlayerGameState.h"

bool CClimbMovement::CanMove() const
{
	if (CCharacterMovement::CanMove() == false)
	{
		return false;
	}
	return true;
}

EMovementResult CClimbMovement::Move(const void* Payload)
{
	CCharacterMovement::Move(Payload);

	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	const FVector3* Dir = static_cast<const FVector3*>(Payload);
	if (OwnerComp == nullptr || Dir == nullptr)
	{
		return EMovementResult::Abort;
	}

	OwnerComp->AddMove(*Dir);
	return EMovementResult::LooseStack;
}
