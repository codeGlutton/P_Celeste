#include "Pch.h"
#include "FastFallMovement.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/SharedGameState.h"

bool CFastFallMovement::CanMove() const
{
	if (CCharacterMovement::CanMove() == false)
	{
		return false;
	}

	return CanFall();
}

EMovementResult CFastFallMovement::Move(const void* Payload)
{
	CCharacterMovement::Move(Payload);

	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	if (UpdateComponent == nullptr)
	{
		return EMovementResult::LooseStack;
	}

	mPreMaxDropSpeed = UpdateComponent->GetMaxDropVelocity();
	UpdateComponent->SetMaxDropVelocity(mPreMaxDropSpeed * mMaxDropRate);
	return EMovementResult::Ongoing;
}

EMovementResult CFastFallMovement::Update(float DeltaTime)
{
	CCharacterMovement::Update(DeltaTime);

	if (CanFall() == false)
	{
		return EMovementResult::Abort;
	}
	return EMovementResult::Ongoing;
}

void CFastFallMovement::Stop()
{
	CCharacterMovement::Stop();

	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	if (UpdateComponent != nullptr)
	{
		UpdateComponent->SetMaxDropVelocity(mPreMaxDropSpeed);
	}
}

bool CFastFallMovement::CanFall() const
{
	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	if (OwnerComp == nullptr)
	{
		return false;
	}
	return OwnerComp->IsLandingOnWall() == false;
}
