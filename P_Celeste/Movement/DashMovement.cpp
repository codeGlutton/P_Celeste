#include "Pch.h"
#include "DashMovement.h"

#include "Character/Character.h"

#include "Component/SceneComponent.h"
#include "Component/Collider.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/SharedGameState.h"

#include "World/TileMainWorld.h"
#include "GameObject/Tile/MapFactoryObject.h"

void CDashMovement::Init(std::weak_ptr<CCharacterMovementComponent> Owner)
{
	CCharacterMovement::Init(Owner);

	FSharedGameState* SharedState = GetSharedGameState();
	if (SharedState != nullptr)
	{
		SharedState->mOnLand.emplace_back([this]() {
			RechargePoint(mDashMaxPoint);
			});
	}

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	if (OwnerComponent != nullptr)
	{
		auto TileWorld = std::static_pointer_cast<CTileMainWorldBase>(OwnerComponent->GetWorld().lock());
		if (TileWorld != nullptr)
		{
			std::shared_ptr<CMapFactoryObject> Factory = TileWorld->GetTileMapFactory().lock();
			if (Factory != nullptr)
			{
				Factory->mOnChangeMap.emplace_back([this]() {
					RechargePoint(mDashMaxPoint);
					});
			}
		}
	}
}

bool CDashMovement::CanMove() const
{
	return CCharacterMovement::CanMove() && mDashPoint > 0;
}

EMovementResult CDashMovement::Update(float DeltaTime)
{
	CCharacterMovement::Update(DeltaTime);

	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	std::shared_ptr<CCollider> MoveTestComponent = GetMoveTestComponent();
	FSharedGameState* SharedState = GetSharedGameState();
	if (UpdateComponent == nullptr || MoveTestComponent == nullptr || SharedState == nullptr)
	{
		return EMovementResult::Abort;
	}

	mDashTime += DeltaTime;
	if (mDashTime > mDashMaxTime)
	{
		ApplyGravity(true);
		MoveTestComponent->SetWorldPos(UpdateComponent->GetWorldPos());
		return EMovementResult::LooseStack;
	}

	if (MoveTestComponent != nullptr && mIsSweep == false)
	{
		if (MoveTestComponent->HasCollisionObject() == false)
		{
			FVector3 CurTestPos = MoveTestComponent->GetWorldPos();
			FVector3 CurPos = UpdateComponent->GetWorldPos();
			FVector3 CurInterpPos = MathUtils::BezierQuad(FVector3::Zero, mMidInterpPos, mEndInterpPos, (mDashTime / mDashMaxTime));

			UpdateComponent->SetWorldPos(CurTestPos);
			MoveTestComponent->SetWorldPos(CurTestPos + CurInterpPos - mPreInterpPos + CurPos - mPrePos);
			
			mPrePos = CurTestPos;
			mPreInterpPos = CurInterpPos;
		}
		else
		{
			SharedState->EffectTouchedWalls(mEndInterpPos, shared_from_this());
			mIsSweep = true;
		}
	}

	return EMovementResult::Ongoing;
}

EMovementResult CDashMovement::Move(const void* Payload)
{
	CCharacterMovement::Move(Payload);

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
	std::shared_ptr<CCollider> MoveTestComponent = GetMoveTestComponent();
	FSharedGameState* SharedState = GetSharedGameState();

	if (OwnerComponent == nullptr || UpdateComponent == nullptr || MoveTestComponent == nullptr || SharedState == nullptr || mDashPoint <= 0)
	{
		return EMovementResult::LooseStack;
	}

	// 평지에서 대시 쓸 때, 약간 띄우기
	if (SharedState->IsLandingOnWall() == true)
	{
		UpdateComponent->AddWorldPos(0.f, 1.f);
	}

	/* 대시 부가 옵션 적용 */

	mDashTime = 0.f;

	ApplyGravity(false);

	SpendPoint();

	FVector3 DashDir;
	if (OwnerComponent->GetLastAccSight().IsZero() == true)
	{
		DashDir = OwnerComponent->GetHorizonSight();
	}
	else
	{
		DashDir = OwnerComponent->GetSight();
	}
	mIsSweep = false;

	mPrePos = UpdateComponent->GetWorldPos();
	mPreInterpPos = FVector3::Zero;

	mEndInterpPos = DashDir * mDashDistance;
	mMidInterpPos = mEndInterpPos / 2.f;

	MoveTestComponent->SetWorldPos(UpdateComponent->GetWorldPos());

	OwnerComponent->GetWorld().lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerDash");

	return EMovementResult::Ongoing;
}

void CDashMovement::Stop()
{
	CCharacterMovement::Stop();

	if (mDashTime <= mDashMaxTime)
	{
		ApplyGravity(true);
	}
}

void CDashMovement::Finish()
{
	CCharacterMovement::Finish();

	FSharedGameState* SharedState = GetSharedGameState();
	if (SharedState != nullptr && SharedState->IsLandingOnWall() == true)
	{
		RechargePoint(mDashMaxPoint);
	}
}

void CDashMovement::ApplyGravity(bool UseGravity)
{
	std::shared_ptr<CSceneComponent> UpdateComp = GetUpdateComponent();
	if (UpdateComp != nullptr)
	{
		UpdateComp->SetUseGravity(UseGravity);
		UpdateComp->ClearPhysics();

		if (UseGravity == true)
		{
			UpdateComp->AddPhysicsVelocity(UpdateComp->GetLastVelocity());
			UpdateComp->SetUseMaxDropVelocity(mPreUseMaxDropVelocity);
		}
		else
		{
			mPreUseMaxDropVelocity = UpdateComp->GetUseMaxDropVelocity();
			UpdateComp->SetUseMaxDropVelocity(false);
		}
	}
}

void CDashMovement::SpendPoint()
{
	mDashPoint = std::max<int32>(0, mDashPoint - 1);
	for (auto& Event : mOnSpendPoint)
	{
		Event(mDashPoint, 1);
	}
}

void CDashMovement::RechargeAllPoint()
{
	RechargePoint(mDashMaxPoint);
}

void CDashMovement::RechargePoint(int32 Point)
{
	int32 PreDashPoint = mDashPoint;
	mDashPoint = std::min<int32>(mDashPoint + Point, mDashMaxPoint);
	if (PreDashPoint != mDashPoint)
	{
		for (auto& Event : mOnRechargePoint)
		{
			Event(mDashPoint, mDashPoint - PreDashPoint);
		}
	}
}

