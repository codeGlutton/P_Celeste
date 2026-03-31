#include "Pch.h"
#include "WallJumpMovement.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/PlayerGameState.h"

#include "World/World.h"
#include "World/WorldAssetManager.h"

bool CWallJumpMovement::CanMove() const
{
	if (CCharacterMovement::CanMove() == false)
	{
		return false;
	}

	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	FSharedGameState* SharedState = GetSharedGameState();
	if (SharedState != nullptr && OwnerComp != nullptr)
	{
		return CJumpMovement::CanMove() == false &&
			SharedState->IsSideOfWall() == true && 
			OwnerComp->IsActiveMovement("Dangle") == false;
	}
	return false;
}

EMovementResult CWallJumpMovement::Move(const void* Payload)
{
	CCharacterMovement::Move(Payload);

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	FPlayerGameState* SharedState = static_cast<FPlayerGameState*>(GetSharedGameState());
	if (UpdateComponent == nullptr || OwnerComponent == nullptr || SharedState == nullptr)
	{
		return EMovementResult::LooseStack;
	}

	mJumpTime = 0.f;
	mPreJumpGravityScale = UpdateComponent->GetGravitySacle();
	UpdateComponent->SetGravitySacle(mJumpGravityScale);

	UpdateComponent->ClearPhysics();
	UpdateComponent->AddPhysicsVelocity(FVector3::Axis[EAxis::Y] * mJumpVelocity);

	bool IsLeftWallJump = SharedState->GetWallState() & ETouchedWall::Flag::Left;
	mWallJumpDir = (IsLeftWallJump == true) ? FVector3::Axis[EAxis::X] : -FVector3::Axis[EAxis::X];

	if (OwnerComponent->GetLastMoveDir().x != 0.f)
	{
		if (IsLeftWallJump == (OwnerComponent->GetLastMoveDir().x < 0))
		{
			mNeedToReverseSight = true;
			SharedState->SetFixSymmetry(!IsLeftWallJump);
			mReverseMoveApplyTime = mJumpMaxTime * 3.5f / 4.f;
		}
	}
	else
	{
		mNeedToReverseSight = false;
		mReverseMoveApplyTime = mJumpMaxTime / 2.f;
	}

	OnStartWallJump();
	OwnerComponent->GetWorld().lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerJump");

	return EMovementResult::Ongoing;
}

EMovementResult CWallJumpMovement::Update(float DeltaTime)
{
	CCharacterMovement::Update(DeltaTime);

	mJumpTime += DeltaTime;

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	
	if (mJumpTime < mReverseMoveApplyTime)
	{
		if (OwnerComponent != nullptr)
		{
			OwnerComponent->AddMove(mWallJumpDir * 2.f);
		}
	}

	if (mJumpTime > mJumpMaxTime)
	{
		std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
		FPlayerGameState* SharedState = static_cast<FPlayerGameState*>(GetSharedGameState());
		if (OwnerComponent != nullptr && UpdateComponent != nullptr && SharedState != nullptr)
		{
			OnEndWallJump();
			if (mNeedToReverseSight = true)
			{
				SharedState->ReleaseFixSymmetry();
			}
			UpdateComponent->SetGravitySacle(mPreJumpGravityScale);
		}
		return EMovementResult::LooseStack;
	}

	return EMovementResult::Ongoing;
}

bool CWallJumpMovement::CanStop(EMovementStopReason Reason) const
{
	if (Reason != EMovementStopReason::Manual)
	{
		return CCharacterMovement::CanStop(Reason);
	}
	return false;
}

void CWallJumpMovement::Stop()
{
	CCharacterMovement::Stop();

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	FPlayerGameState* SharedState = static_cast<FPlayerGameState*>(GetSharedGameState());
	if (OwnerComponent != nullptr && UpdateComponent != nullptr && SharedState != nullptr)
	{
		OnEndWallJump();
		if (mNeedToReverseSight = true)
		{
			SharedState->ReleaseFixSymmetry();
		}
		UpdateComponent->SetGravitySacle(mPreJumpGravityScale);
	}
}

void CWallJumpMovement::OnStartWallJump()
{
	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	if (OwnerComponent != nullptr)
	{
		OwnerComponent->AddMovementTag("WallJump");
		for (auto& Event : mOnStartWallJump)
		{
			Event(mWallJumpDir, mNeedToReverseSight);
		}
	}
}

void CWallJumpMovement::OnEndWallJump()
{
	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	if (OwnerComponent != nullptr)
	{
		OwnerComponent->RemoveMovementTag("WallJump");
	}
}

