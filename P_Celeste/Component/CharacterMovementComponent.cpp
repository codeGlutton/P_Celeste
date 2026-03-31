#include "Pch.h"
#include "CharacterMovementComponent.h"

#include "Movement/SharedGameState.h"
#include "Movement/CharacterMovement.h"

CCharacterMovementComponent* CCharacterMovementComponent::Clone() const
{
	return new CCharacterMovementComponent(*this);
}

void CCharacterMovementComponent::Update(float DeltaTime)
{
	CObjectMovementComponent::Update(DeltaTime);

	if (mSharedState != nullptr)
	{
		mSharedState->Update(DeltaTime);
	}

	std::vector<std::string> FinishMovementNames;
	for (auto& ActiveMovementPair : mActiveMovements)
	{
		auto& Movements = mMovements[ActiveMovementPair.first];
		for (auto& Movement : Movements)
		{
			if (Movement->IsOngoing() == true)
			{
				EMovementResult Result = Movement->RequestToUpdateMove(DeltaTime);

				if (Result == EMovementResult::Abort)
				{
					bool IsStop = false;
					Movement->RequestToAbortMove(EMovementStopReason::Self, OUT IsStop);
					if (IsStop == true)
					{
						--ActiveMovementPair.second;
					}
				}
				else if (Result == EMovementResult::LooseStack)
				{
					bool IsFinish = false;
					Movement->RequestToLooseStack(OUT IsFinish);
					if (IsFinish == true)
					{
						--ActiveMovementPair.second;
					}
				}
			}
		}

		if (ActiveMovementPair.second == 0)
		{
			FinishMovementNames.push_back(ActiveMovementPair.first);
		}
	}

	for (const std::string& FinishMovementName : FinishMovementNames)
	{
		mActiveMovements.erase(FinishMovementName);
	}
}

void CCharacterMovementComponent::PostUpdate(float DeltaTime)
{
	CObjectMovementComponent::PostUpdate(DeltaTime);

	if (mAccSightDir.IsZero() == false)
	{
		mSightDir = mAccSightDir;
		mSightDir.Normalize();
		if (mAccSightDir.x != 0.f)
		{
			mHorizonSightDir.x = mAccSightDir.x;
			mHorizonSightDir.y = mHorizonSightDir.z = 0.f;
			mHorizonSightDir.Normalize();
		}
	}
}

void CCharacterMovementComponent::PostRender()
{
	CObjectMovementComponent::PostRender();

	mLastAccSightDir = mAccSightDir;
	mLastAccSightDir.Normalize();

	mAccSightDir = FVector3::Zero;

	mLastHorizonSightDir = mHorizonSightDir;
}

FMovementHandle CCharacterMovementComponent::AddMovement(std::shared_ptr<CCharacterMovement> Movement)
{
	Movement->Init(GetSelf<CCharacterMovementComponent>());
	mMovements[Movement->GetName()].emplace_back(Movement);
	return FMovementHandle{ mMovements[Movement->GetName()].size() - 1, Movement->GetName() };
}

int32 CCharacterMovementComponent::DoMovement(const std::string& Name, const void* Payload)
{
	if (mApplyMove == false || mMovements.find(Name) == mMovements.end())
	{
		return 0;
	}

	std::vector<std::shared_ptr<CCharacterMovement>*> ExecMovements;
	std::unordered_set<std::string> AllLooseTags;
	for (auto& Movement : mMovements[Name])
	{
		bool IsCanMove = false;
		const std::vector<std::string>* LooseTags = nullptr;
		Movement->RequestToCanMove(OUT IsCanMove, OUT LooseTags);
		if (IsCanMove == true)
		{
			ExecMovements.push_back(&Movement);
			for (const std::string& Tag : *LooseTags)
			{
				AllLooseTags.insert(Tag);
			}
		}
	}

	for (const std::string& Tag : AllLooseTags)
	{
		if (mActiveMovements.find(Tag) != mActiveMovements.end())
		{
			for (auto& Movement : mMovements[Tag])
			{
				if (Movement->IsOngoing() == true)
				{
					bool IsStop = false;
					Movement->RequestToAbortMove(EMovementStopReason::Lock, OUT IsStop);
					if (IsStop == true)
					{
						--mActiveMovements[Tag];
					}
				}
			}

			if (mActiveMovements[Tag] == 0)
			{
				mActiveMovements.erase(Tag);
			}
		}
	}

	int32 ActiveCount = 0;
	for (std::shared_ptr<CCharacterMovement>* MovementPtr : ExecMovements)
	{
		std::shared_ptr<CCharacterMovement>& Movement = *MovementPtr;

		bool IsNewStart = false;
		EMovementResult Result = Movement->RequestToStartMove(Payload, OUT IsNewStart);
		if (IsNewStart == false)
		{
			continue;
		}
		++mActiveMovements[Name];
		++ActiveCount;

		if (Result == EMovementResult::Abort)
		{
			bool IsStop = false;
			Movement->RequestToAbortMove(EMovementStopReason::Self, OUT IsStop);
			if (IsStop == true)
			{
				--mActiveMovements[Name];
			}
		}
		else if (Result == EMovementResult::LooseStack)
		{
			bool IsFinish = false;
			Movement->RequestToLooseStack(OUT IsFinish);
			if (IsFinish == true)
			{
				--mActiveMovements[Name];
			}
		}
	}

	if (mActiveMovements[Name] == 0)
	{
		mActiveMovements.erase(Name);
	}

	return ActiveCount;
}

int32 CCharacterMovementComponent::StopMovement(const std::string& Name)
{
	if (mMovements.find(Name) == mMovements.end() || mActiveMovements.find(Name) == mActiveMovements.end())
	{
		return 0;
	}

	int32 StopCount = 0;
	for (auto& Movement : mMovements[Name])
	{
		if (Movement->IsOngoing() == true)
		{
			bool IsStop = false;
			Movement->RequestToAbortMove(EMovementStopReason::Manual, OUT IsStop);
			if (IsStop == true)
			{
				++StopCount;
				--mActiveMovements[Name];
			}
		}
	}
	if (StopCount > 0 && mActiveMovements[Name] == 0)
	{
		mActiveMovements.erase(Name);
	}
	return StopCount;
}

int32 CCharacterMovementComponent::StopAllMovement()
{
	int32 StopCount = 0;
	for (auto& MovementPair : mMovements)
	{
		const int32 PreStopCount = StopCount;
		for (auto& Movement : MovementPair.second)
		{
			if (Movement->IsOngoing() == true)
			{
				bool IsStop = false;
				Movement->RequestToAbortMove(EMovementStopReason::Manual, OUT IsStop);
				if (IsStop == true)
				{
					++StopCount;
					--mActiveMovements[MovementPair.first];
				}
			}
		}
		if (StopCount > PreStopCount && mActiveMovements[MovementPair.first] == 0)
		{
			mActiveMovements.erase(MovementPair.first);
		}
	}
	return StopCount;
}

void CCharacterMovementComponent::AddMovementTag(const std::string& Name)
{
	if (mLockMovementTags.find(Name) != mLockMovementTags.end())
	{
		return;
	}

	++mActiveMovements[Name];
}

void CCharacterMovementComponent::RemoveMovementTag(const std::string& Name)
{
	if (mActiveMovements.find(Name) == mActiveMovements.end())
	{
		return;
	}

	--mActiveMovements[Name];
	if (mActiveMovements[Name] == 0)
	{
		mActiveMovements.erase(Name);
	}
}

bool CCharacterMovementComponent::IsSideOfWall() const
{
	if (mSharedState == nullptr)
	{
		return false;
	}
	return mSharedState->IsSideOfWall();
}

bool CCharacterMovementComponent::IsLandingOnWall() const
{
	if (mSharedState == nullptr)
	{
		return false;
	}
	return mSharedState->IsLandingOnWall();
}

bool CCharacterMovementComponent::IsCrushed() const
{
	if (mSharedState == nullptr)
	{
		return false;
	}
	return mSharedState->IsCrushed();
}

void CCharacterMovementComponent::LockTags(const std::vector<std::string>& LockTags)
{
	for (const std::string& Tag : LockTags)
	{
		++mLockMovementTags[Tag];
	}
}

void CCharacterMovementComponent::UnlockTags(const std::vector<std::string>& LockTags)
{
	for (const std::string& Tag : LockTags)
	{
		--mLockMovementTags[Tag];
		if (mLockMovementTags[Tag] == 0)
		{
			mLockMovementTags.erase(Tag);
		}
	}
}

bool CCharacterMovementComponent::MatchTags(const std::string& Name, const std::vector<std::string>& RequiredTags, const std::vector<std::string>& ForbiddenTags) const
{
	// 락 걸린 태그는 재생불가
	if (mLockMovementTags.find(Name) != mLockMovementTags.end())
	{
		return false;
	}

	// 요구 사항 태그가 있는지
	for (const std::string& Tag : RequiredTags)
	{
		if (mActiveMovements.find(Tag) == mActiveMovements.end())
		{
			return false;
		}
	}
	// 금지 사항 태그가 없는지
	for (const std::string& Tag : ForbiddenTags)
	{
		if (mActiveMovements.find(Tag) != mActiveMovements.end())
		{
			return false;
		}
	}
	return true;
}
