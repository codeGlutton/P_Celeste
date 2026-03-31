#include "Pch.h"
#include "JumpMovement.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/SharedGameState.h"

#include "World/World.h"
#include "World/WorldAssetManager.h"

bool CJumpMovement::CanMove() const
{
	if (CCharacterMovement::CanMove() == false)
	{
		return false;
	}

	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	FSharedGameState* SharedState = GetSharedGameState();
	if (SharedState != nullptr && OwnerComp != nullptr)
	{
		return SharedState->IsLandingOnWall() == true ||
			(SharedState->IsSideOfWall() == true && OwnerComp->IsActiveMovement("Dangle") == true);
	}
	return false;
}

EMovementResult CJumpMovement::Update(float DeltaTime)
{
	CCharacterMovement::Update(DeltaTime);

	mJumpTime += DeltaTime;

	if (mJumpTime > mJumpMaxTime)
	{
		std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
		if (UpdateComponent != nullptr)
		{
			UpdateComponent->SetGravitySacle(mPreJumpGravityScale);
		}
		return EMovementResult::LooseStack;
	}
	return EMovementResult::Ongoing;
}

EMovementResult CJumpMovement::Move(const void* Payload)
{
	CCharacterMovement::Move(Payload);

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	FSharedGameState* SharedState = GetSharedGameState();
	if (UpdateComponent == nullptr || OwnerComponent == nullptr || SharedState == nullptr)
	{
		return EMovementResult::LooseStack;
	}

	mJumpTime = 0.f;
	mPreJumpGravityScale = UpdateComponent->GetGravitySacle();
	UpdateComponent->SetGravitySacle(mJumpGravityScale);

	UpdateComponent->ClearPhysics();
	UpdateComponent->AddPhysicsVelocity(FVector3::Axis[EAxis::Y] * mJumpVelocity);

	OwnerComponent->GetWorld().lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerJump");

	return EMovementResult::Ongoing;
}

void CJumpMovement::Stop()
{
	CCharacterMovement::Stop();

	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	if (UpdateComponent != nullptr)
	{
		UpdateComponent->SetGravitySacle(mPreJumpGravityScale);
	}
}

