#include "Pch.h"
#include "SlideMovement.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/SharedGameState.h"

#include "World/World.h"
#include "World/WorldAssetManager.h"

bool CSlideMovement::CanMove() const
{
	if (CCharacterMovement::CanMove() == false)
	{
		return false;
	}

	return CanSlide();
}

EMovementResult CSlideMovement::Move(const void* Payload)
{
	CCharacterMovement::Move(Payload);

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	if (OwnerComponent == nullptr)
	{
		return EMovementResult::LooseStack;
	}

	mIsInterpedToStart = true;
	OwnerComponent->GetWorld().lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerSlide");

	return EMovementResult::Ongoing;
}

EMovementResult CSlideMovement::Update(float DeltaTime)
{
	CCharacterMovement::Update(DeltaTime);

	if (CanSlide() == false)
	{
		return EMovementResult::Abort;
	}

	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	if (UpdateComponent == nullptr)
	{
		return EMovementResult::Abort;
	}

	float DropVelocity = UpdateComponent->GetPhysicsVelocity().y;
	if (mIsInterpedToStart == true)
	{
		float FrictionVelocity = mSlideStartFrictionAccel * DeltaTime * UpdateComponent->GetGravitySacle();
		float ResultDropVelocity = DropVelocity + FrictionVelocity;
		if (ResultDropVelocity > -mSlideMaxStartVelocity)
		{
			mIsInterpedToStart = false;
			ResultDropVelocity = -mSlideMaxStartVelocity;
		}
		UpdateComponent->AddPhysicsVelocity(FVector3(0.f, ResultDropVelocity - DropVelocity, 0.f));
	}
	else
	{
		float FrictionVelocity = mSlideFrictionAccel * DeltaTime * UpdateComponent->GetGravitySacle();
		float ResultDropVelocity = std::max<float>(
			DropVelocity + FrictionVelocity, -mSlideMinDropVelocity
		);
		UpdateComponent->AddPhysicsVelocity(FVector3(0.f, ResultDropVelocity - DropVelocity, 0.f));
	}

	return EMovementResult::Ongoing;
}

void CSlideMovement::Stop()
{
	CCharacterMovement::Stop();

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	if (OwnerComponent == nullptr)
	{
		return;
	}

	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = OwnerComponent->GetWorld().lock()->GetWorldAssetManager().lock();
	if (WorldAssetMgr->IsSoundPlaying("S_PlayerSlide") == true)
	{
		OwnerComponent->GetWorld().lock()->GetWorldAssetManager().lock()->SoundStop("S_PlayerSlide");
	}
}

bool CSlideMovement::CanSlide() const
{
	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	std::shared_ptr<CSceneComponent> UpdateComp = GetUpdateComponent();
	FSharedGameState* SharedState = GetSharedGameState();
	if (OwnerComp == nullptr || SharedState == nullptr || UpdateComp == nullptr)
	{
		return false;
	}

	if (OwnerComp->GetLastMoveDir().IsZero() == true || OwnerComp->IsLandingOnWall() == true)
	{
		return false;
	}

	return (OwnerComp->IsSideOfWall() == true) &&
		(OwnerComp->GetHorizonSight().x < 0.f) == ((SharedState->GetWallState() & ETouchedWall::Flag::Left) != 0) &&
		(UpdateComp->GetPhysicsVelocity().y < 0.f);
}
