#include "Pch.h"
#include "CharacterMovement.h"

#include "Character/Character.h"
#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "GameObject/Interator/InteractableObject.h"

CMovementEffector::CMovementEffector(std::weak_ptr<CGameObject> Interactor, const std::unordered_map<const CCollider*, std::weak_ptr<CInteractableObject>>& DynamicWallMap, std::weak_ptr<CCharacterMovement> Movement) :
	mInteractor(Interactor),
	mMovement(Movement)
{
	mInteractableObjects.reserve(DynamicWallMap.size());
	for (auto& WallPair : DynamicWallMap)
	{
		std::shared_ptr<CInteractableObject> DynamicWall = WallPair.second.lock();
		if (DynamicWall != nullptr)
		{
			DynamicWall->OnEffectedByMovement(mInteractor.lock(), mMovement.lock());
			mInteractableObjects.push_back(DynamicWall);
		}
	}
}

CMovementEffector::~CMovementEffector()
{
	for (auto& WallObject : mInteractableObjects)
	{
		std::shared_ptr<CInteractableObject> DynamicWall = WallObject.lock();
		if (DynamicWall != nullptr)
		{
			DynamicWall->OnFinishedByMovement(mInteractor.lock(), mMovement.lock());
		}
	}
}

void CCharacterMovement::RequestToCanMove(OUT bool& IsCanMove, OUT const std::vector<std::string>*& LooseTags)
{
	LooseTags = &mLooseTagsOnMove;

	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	if (OwnerComp == nullptr)
	{
		IsCanMove = false;
		return;
	}
	IsCanMove = OwnerComp->MatchTags(mName, mRequiredTagsOnMove, mForbiddenTagsOnMove) && CanMove();
}

EMovementResult CCharacterMovement::RequestToStartMove(const void* Payload, OUT bool& IsNewMove)
{
	EMovementResult Result = EMovementResult::None;

	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();

	IsNewMove = !mIsOngoing;
	if (mIsOngoing == true)
	{
		switch (mRecallRule)
		{
		case EMovementRecallRule::Restart:
			mIsOngoing = false;
			--mStack;
			Stop();
			Finish();
			OnFinish();

			mIsOngoing = true;
			++mStack;
			Result = Move(Payload);
			OnMove();
			break;
		case EMovementRecallRule::Stacking:
			++mStack;
			break;
		case EMovementRecallRule::Ignore:
			break;
		}
	}
	else
	{
		OwnerComp->LockTags(mLockTagsOnUpdate);

		mIsOngoing = true;
		++mStack;
		Result = Move(Payload);
		OnMove();
		if (mRecallRule == EMovementRecallRule::Stacking)
		{
			ApplyStack();
		}
	}
	return Result;
}

EMovementResult CCharacterMovement::RequestToUpdateMove(float DeltaTime)
{
	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	if (OwnerComp == nullptr)
	{
		return EMovementResult::Abort;
	}

	bool IsFailed = OwnerComp->MatchTags(mName, mRequiredTagsOnUpdate, mForbiddenTagsOnUpdate);
	if (IsFailed == false)
	{
		return EMovementResult::Abort;
	}
	return Update(DeltaTime);
}

void CCharacterMovement::RequestToLooseStack(OUT bool& IsFinish)
{
	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	if (OwnerComp == nullptr)
	{
		return;
	}

	--mStack;

	if (mRecallRule == EMovementRecallRule::Stacking)
	{
		LooseStack();
		if (mStack > 0)
		{
			ApplyStack();

			IsFinish = false;
			return;
		}
	}

	mIsOngoing = false;
	Finish();
	OnFinish();
	OwnerComp->UnlockTags(mLockTagsOnUpdate);

	IsFinish = true;
	return;
}

void CCharacterMovement::RequestToAbortMove(EMovementStopReason Reason, OUT bool& IsStop)
{
	std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
	if (OwnerComp == nullptr)
	{
		IsStop = false;
		return;
	}

	IsStop = CanStop(Reason);
	if (IsStop == true)
	{
		mStack = 0;
		mIsOngoing = false;
		Stop();
		Finish();
		OnFinish();
		OwnerComp->UnlockTags(mLockTagsOnUpdate);
	}
}

std::shared_ptr<CCharacterMovementComponent> CCharacterMovement::GetOwnerComponent() const
{
	return mOwnerComponent.lock();
}

std::shared_ptr<CSceneComponent> CCharacterMovement::GetUpdateComponent() const
{
	std::shared_ptr<CCharacterMovementComponent> MovementComp = GetOwnerComponent();
	if (MovementComp != nullptr)
	{
		return MovementComp->GetUpdateComponent().lock();
	}
	return nullptr;
}

std::shared_ptr<CCollider> CCharacterMovement::GetMoveTestComponent() const
{
	std::shared_ptr<CCharacterMovementComponent> MovementComp = GetOwnerComponent();
	if (MovementComp != nullptr)
	{
		return MovementComp->GetMoveTestComponent().lock();
	}
	return nullptr;
}
std::shared_ptr<CCharacter> CCharacterMovement::GetOwnerCharacter() const
{
	std::shared_ptr<CCharacterMovementComponent> MovementComp = GetOwnerComponent();
	if (MovementComp != nullptr)
	{
		return std::static_pointer_cast<CCharacter>(MovementComp->GetOwner().lock());
	}
	return nullptr;
}

FSharedGameState* CCharacterMovement::GetSharedGameState() const
{
	std::shared_ptr<CCharacterMovementComponent> MovementComp = GetOwnerComponent();
	if (MovementComp != nullptr)
	{
		return &MovementComp->GetSharedState<FSharedGameState>();
	}
	return nullptr;
}

void CCharacterMovement::OnMove() const
{
	for (auto& Event : mOnMove)
	{
		Event();
	}
}

void CCharacterMovement::OnFinish() const
{
	for (auto& Event : mOnFinish)
	{
		Event();
	}
}

