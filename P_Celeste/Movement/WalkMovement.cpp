#include "Pch.h"
#include "WalkMovement.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/SharedGameState.h"

bool CWalkMovement::CanMove() const
{
	if (CCharacterMovement::CanMove() == false)
	{
		return false;
	}

	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	if (OwnerComp != nullptr)
	{
		return OwnerComp->IsActiveMovement("Dangle") == false;
	}
	return false;
}

EMovementResult CWalkMovement::Move(const void* Payload)
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
