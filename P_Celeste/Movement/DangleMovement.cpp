#include "Pch.h"
#include "DangleMovement.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/PlayerGameState.h"

#include "World/World.h"
#include "World/WorldAssetManager.h"

void CDangleMovement::Init(std::weak_ptr<CCharacterMovementComponent> Owner)
{
	CCharacterMovement::Init(Owner);

	FSharedGameState* SharedState = GetSharedGameState();
	if (SharedState != nullptr)
	{
		SharedState->mOnLand.emplace_back([this]() {
			RechargeAllPoint();
			});
	}
}

bool CDangleMovement::CanMove() const
{
	if (CCharacterMovement::CanMove() == false)
	{
		return false;
	}

	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	FSharedGameState* SharedState = GetSharedGameState();
	if (SharedState != nullptr && OwnerComp != nullptr)
	{
		return SharedState->IsSideOfWall() == true &&
			((SharedState->GetWallState() & ETouchedWall::Flag::Left) != 0) == (OwnerComp->GetHorizonSight().x < 0.f) &&
			(mDangleTime < mDangleMaxTime);
	}
	return false;
}

EMovementResult CDangleMovement::Update(float DeltaTime)
{
	CCharacterMovement::Update(DeltaTime);

	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	FSharedGameState* SharedState = GetSharedGameState();
	if (SharedState == nullptr || OwnerComp == nullptr)
	{
		return EMovementResult::Abort;
	}

	if (OwnerComp->GetLastMoveDir().y > 0.f)
	{
		DeltaTime *= 5.f;
	}
	mDangleTime += DeltaTime;

	if (mWasWarn == false && mDangleTime > mDangleWarningTime)
	{
		mWasWarn = true;
		for (auto& Event : mOnWarnLimit)
		{
			Event(mDangleTime);
		}
	}
	if (SharedState->IsSideOfWall() == false || mDangleTime > mDangleMaxTime)
	{
		return EMovementResult::Abort;
	}
	return EMovementResult::Ongoing;
}

EMovementResult CDangleMovement::Move(const void* Payload)
{
	CCharacterMovement::Move(Payload);

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	FPlayerGameState* SharedState = static_cast<FPlayerGameState*>(GetSharedGameState());
	if (OwnerComponent == nullptr || SharedState == nullptr || SharedState->IsFixedSymmetry() == true)
	{
		return EMovementResult::LooseStack;
	}

	const bool IsSymmetry = OwnerComponent->GetHorizonSight().x < 0;
	const ETouchedWall::Type TouchedType = (IsSymmetry == true) ? ETouchedWall::Type::Left : ETouchedWall::Type::Right;
	mEffector = SharedState->EffectTouchedWalls(TouchedType, shared_from_this());
	SharedState->SetFixSymmetry(IsSymmetry);
	mPreSpeed = OwnerComponent->GetSpeed();
	OwnerComponent->SetSpeed(mPreSpeed * mDangleSpeedRate);
	ApplyGravity(false);

	mDangleTime += mDangleSpendTime;

	int32 Idx = std::rand() % 5;
	OwnerComponent->GetWorld().lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerGrab" + std::to_string(Idx));

	return EMovementResult::Ongoing;
}

void CDangleMovement::Stop()
{
	CCharacterMovement::Stop();

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	FPlayerGameState* SharedState = static_cast<FPlayerGameState*>(GetSharedGameState());
	
	ApplyGravity(true);
	if (OwnerComponent != nullptr)
	{
		OwnerComponent->SetSpeed(mPreSpeed);
	}
	if (SharedState != nullptr && SharedState->IsFixedSymmetry() == true)
	{
		SharedState->ReleaseFixSymmetry();
		if (UpdateComponent != nullptr && OwnerComponent->GetLastMoveDir().y > 0.f)
		{
			ETouchedWall::Flag PreTag = (mDangleDir.x < 0) ? ETouchedWall::Flag::Left : ETouchedWall::Flag::Right;
			if ((SharedState->GetWallState() & PreTag) == 0)
			{
				UpdateComponent->AddPhysicsVelocity(FVector3::Axis[EAxis::Y] * mReleaseJumpSpeed);
			}
		}
	}

	mDangleDir = FVector3::Zero;
	mEffector.reset();
}

void CDangleMovement::RechargeAllPoint()
{
	float PreDangleTime = mDangleTime;
	mDangleTime = 0.f;
	mWasWarn = false;
	if (PreDangleTime > 0.f)
	{
		for (auto& Event : mOnRechargePoint)
		{
			Event(PreDangleTime);
		}
	}
}

void CDangleMovement::ApplyGravity(bool UseGravity)
{
	std::shared_ptr<CSceneComponent> UpdateComp = GetUpdateComponent();
	if (UpdateComp != nullptr)
	{
		UpdateComp->SetUseGravity(UseGravity);
		UpdateComp->ClearPhysics();
	}
}
