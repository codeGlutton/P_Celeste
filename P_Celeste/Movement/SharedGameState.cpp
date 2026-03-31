#include "Pch.h"
#include "SharedGameState.h"

#include "Component/Collider.h"
#include "GameObject/Interator/InteractableObject.h"

#include "Movement/CharacterMovement.h"

void FSharedGameState::Update(float DeltaTime)
{
	if (mWallState & ETouchedWall::Flag::Down && mIsLand == false)
	{
		mAccLandingTime += DeltaTime;
		if (mAccLandingTime >= mLandingThresholdTime)
		{
			mIsLand = true;
			for (auto& Event : mOnLand)
			{
				Event();
			}

			const std::size_t TypeIndex = static_cast<std::size_t>(ETouchedWall::Type::Down);
			for (auto& DynamicDownWallPair : mTouchedDynamicWalls[TypeIndex])
			{
				std::shared_ptr<CInteractableObject> DynamicDownWall = DynamicDownWallPair.second.lock();
				DynamicDownWall->OnLanded(mOwnerObject.lock());
			}
		}
	}
}

void FSharedGameState::AddTouchedWall(ETouchedWall::Type TouchType, const CCollider* Collider)
{
	const std::size_t TypeIndex = static_cast<std::size_t>(TouchType);
	
	++mTouchedWallCounts[TypeIndex];
	if (mTouchedWallCounts[TypeIndex] == 1)
	{
		mWallState |= static_cast<ETouchedWall::Flag>(1 << static_cast<uint8>(TouchType));
		if (TouchType == ETouchedWall::Type::Down)
		{
			mIsLand = false;
			mAccLandingTime = 0.f;
		}
	}

	if (Collider->GetStatic() == false)
	{
		auto InteractableObject = std::static_pointer_cast<CInteractableObject>(Collider->GetOwner().lock());
		if (InteractableObject != nullptr)
		{
			mTouchedDynamicWalls[TypeIndex][Collider] = InteractableObject;

			InteractableObject->OnTouchedDown(TouchType, mOwnerObject.lock());
			if (mIsLand == true && TouchType == ETouchedWall::Type::Down)
			{
				InteractableObject->OnLanded(mOwnerObject.lock());
			}
		}
	}
}

void FSharedGameState::RemoveTouchedWall(ETouchedWall::Type TouchType, const CCollider* Collider)
{
	const std::size_t TypeIndex = static_cast<std::size_t>(TouchType);

	if (Collider->GetStatic() == false)
	{
		auto InteractableObject = std::static_pointer_cast<CInteractableObject>(Collider->GetOwner().lock());
		if (InteractableObject != nullptr)
		{
			if (mIsLand == true && TouchType == ETouchedWall::Type::Down)
			{
				InteractableObject->OnJumped(mOwnerObject.lock());
			}
			InteractableObject->OnTouchedUp(TouchType, mOwnerObject.lock());

			mTouchedDynamicWalls[TypeIndex].erase(Collider);
		}
	}

	--mTouchedWallCounts[TypeIndex];
	if (mTouchedWallCounts[TypeIndex] == 0)
	{
		mWallState &= static_cast<ETouchedWall::Flag>(~(1 << static_cast<uint8>(TouchType)));
		if (TouchType == ETouchedWall::Type::Down && mIsLand == true)
		{
			mIsLand = false;
			mAccLandingTime = 0.f;
			for (auto& Event : mOnAir)
			{
				Event();
			}
		}
	}
}

std::shared_ptr<CMovementEffector> FSharedGameState::EffectTouchedWalls(ETouchedWall::Type TouchType, std::shared_ptr<CCharacterMovement> Movement)
{
	const std::size_t TypeIndex = static_cast<std::size_t>(TouchType);
	return std::make_shared<CMovementEffector>(mOwnerObject, mTouchedDynamicWalls[TypeIndex], Movement);
}

std::shared_ptr<CMovementEffector> FSharedGameState::EffectTouchedWalls(const FVector3& TouchDir, std::shared_ptr<CCharacterMovement> Movement)
{
	ETouchedWall::Type TouchType;

	FVector3 DirNormal = FVector3::Normalize(TouchDir);
	float XDotResult = DirNormal.Dot(FVector3::Axis[EAxis::X]);
	if (XDotResult < -0.7f)
	{
		TouchType = ETouchedWall::Type::Left;
	}
	else if (XDotResult > 0.7f)
	{
		TouchType = ETouchedWall::Type::Right;
	}
	else
	{
		TouchType = DirNormal.Dot(FVector3::Axis[EAxis::Y]) > 0.f ? ETouchedWall::Type::Up : ETouchedWall::Type::Down;
	}

	return EffectTouchedWalls(TouchType, Movement);
}

bool FSharedGameState::IsCrushed() const
{
	if ((mWallState & ETouchedWall::HorizonHit) == ETouchedWall::HorizonHit)
	{
		auto& LeftWallMap = mTouchedDynamicWalls[static_cast<std::size_t>(ETouchedWall::Type::Left)];
		auto& RightWallMap = mTouchedDynamicWalls[static_cast<std::size_t>(ETouchedWall::Type::Right)];

		for (auto& LeftWallPair : LeftWallMap)
		{
			if (LeftWallPair.second.expired() == false && LeftWallPair.first->GetOwner().lock()->GetVelocity().x > 0.f)
			{
				return true;
			}
		}
		for (auto& RightWallPair : RightWallMap)
		{
			if (RightWallPair.second.expired() == false && RightWallPair.first->GetOwner().lock()->GetVelocity().x < 0.f)
			{
				return true;
			}
		}
	}
	if ((mWallState & ETouchedWall::VerticalHit) == ETouchedWall::VerticalHit)
	{
		auto& UpWallMap = mTouchedDynamicWalls[static_cast<std::size_t>(ETouchedWall::Type::Up)];
		auto& DownWallMap = mTouchedDynamicWalls[static_cast<std::size_t>(ETouchedWall::Type::Down)];

		for (auto& UpWallPair : UpWallMap)
		{
			if (UpWallPair.second.expired() == false && UpWallPair.first->GetOwner().lock()->GetVelocity().y < 0.f)
			{
				return true;
			}
		}
		for (auto& DownWallPair : DownWallMap)
		{
			if (DownWallPair.second.expired() == false && DownWallPair.first->GetOwner().lock()->GetVelocity().y > 0.f)
			{
				return true;
			}
		}
	}
	return false;
}
