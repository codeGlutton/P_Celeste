#include "Pch.h"
#include "DeadMovement.h"

#include "Character/Character.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "World/World.h"
#include "World/WorldAssetManager.h"

EMovementResult CDeadMovement::Move(const void* Payload)
{
	CCharacterMovement::Move(Payload);

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	std::shared_ptr<CCharacter> OwnerChar = GetOwnerCharacter();
	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	if (Payload == nullptr || OwnerComponent == nullptr || OwnerChar == nullptr || UpdateComponent == nullptr)
	{
		return EMovementResult::LooseStack;
	}

	mRespawnDelayTime = *static_cast<const float*>(Payload);
	mRespawnAccTime = 0.f;

	OwnerChar->DetachGameObject(OwnerChar->GetAttachedParent());
	OwnerComponent->SetApplyMove(false);

	UpdateComponent->SetSimulatePhysics(false);
	UpdateComponent->SetUseGravity(false);
	UpdateComponent->ClearPhysics();

	for (auto& Event : mOnDead)
	{
		Event();
	}

	OwnerComponent->GetWorld().lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerDeath");

	return EMovementResult::Ongoing;
}

EMovementResult CDeadMovement::Update(float DeltaTime)
{
	CCharacterMovement::Update(DeltaTime);

	mRespawnAccTime += DeltaTime;
	if (mRespawnAccTime > mRespawnDelayTime)
	{
		return EMovementResult::Abort;
	}
	return EMovementResult::Ongoing;
}

void CDeadMovement::Stop()
{
	CCharacterMovement::Stop();

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	if (OwnerComponent == nullptr || UpdateComponent == nullptr)
	{
		return;
	}
	OwnerComponent->SetApplyMove(true);

	UpdateComponent->SetSimulatePhysics(true);
	UpdateComponent->SetUseGravity(true);
	UpdateComponent->ClearPhysics();

	for (auto& Event : mOnRevive)
	{
		Event();
	}
}

