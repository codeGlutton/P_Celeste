#pragma once

#include "Component/ObjectMovementComponent.h"
#include "Movement/MovementInfo.h"

class CCollider;
struct FSharedGameState;
class CCharacterMovement;

class CCharacterMovementComponent : public CObjectMovementComponent
{
	friend class CGameObject;
	friend class CObject;

protected:
	CCharacterMovementComponent() = default;
	CCharacterMovementComponent(const CCharacterMovementComponent& ref) = default;
	CCharacterMovementComponent(CCharacterMovementComponent&& ref) noexcept = default;

public:
	virtual ~CCharacterMovementComponent() override = default;

protected:
	virtual CCharacterMovementComponent* Clone() const override;

public:
	virtual void Update(float DeltaTime) override;
	virtual void PostUpdate(float DeltaTime) override;
	virtual void PostRender();

public:
	void AddSight(const FVector3& Dir)
	{
		mAccSightDir += Dir;
	}
	void SetSight(const FVector3& Dir)
	{
		mAccSightDir = Dir;
	}

	void SetSharedState(std::shared_ptr<FSharedGameState> State)
	{
		mSharedState = std::move(State);
	}

	void SetMoveTestComponent(std::weak_ptr<CCollider> Collider)
	{
		mMoveTestCollider = std::move(Collider);
	}

	FMovementHandle AddMovement(std::shared_ptr<CCharacterMovement> Movement);

	int32 DoMovement(const std::string& Name, const void* Payload = nullptr);
	int32 StopMovement(const std::string& Name);
	int32 StopAllMovement();

	void AddMovementTag(const std::string& Name);
	void RemoveMovementTag(const std::string& Name);

public:
	const FVector3& GetLastAccSight() const
	{
		return mLastAccSightDir;
	}
	const FVector3& GetSight() const
	{
		return mSightDir;
	}
	const FVector3& GetHorizonSight() const
	{
		return mHorizonSightDir;
	}
	const FVector3& GetLastHorizonSight() const
	{
		return mLastHorizonSightDir;
	}
	bool IsSideOfWall() const;
	bool IsLandingOnWall() const;
	bool IsCrushed() const;

	const std::weak_ptr<CCollider>& GetMoveTestComponent() const
	{
		return mMoveTestCollider;
	}

	template<typename T>
	T& GetSharedState() const
	{
		return *static_cast<T*>(mSharedState.get());
	}
	FSharedGameState& GetSharedState() const
	{
		return *mSharedState.get();
	}

	template<typename T>
	std::shared_ptr<T> GetMovement(const FMovementHandle& Handle) const
	{
		return std::static_pointer_cast<T>(GetMovement(Handle));
	}
	const std::shared_ptr<CCharacterMovement>& GetMovement(const FMovementHandle& Handle) const
	{
		if (mMovements.find(Handle.mName) == mMovements.end())
		{
			return nullptr;
		}
		return mMovements.at(Handle.mName).at(Handle.mIndex);
	}

	bool IsActiveMovement(const std::string& Name) const
	{
		return mActiveMovements.find(Name) != mActiveMovements.end();
	}

public:
	void LockTags(const std::vector<std::string>& LockTags);
	void UnlockTags(const std::vector<std::string>& LockTags);
	bool MatchTags(const std::string& Name, const std::vector<std::string>& RequiredTags, const std::vector<std::string>& ForbiddenTags) const;

protected:
	FVector3 mAccSightDir = FVector3::Zero;
	FVector3 mLastAccSightDir = FVector3::Zero;

	FVector3 mSightDir = FVector3::Axis[EAxis::X];
	FVector3 mHorizonSightDir = FVector3::Axis[EAxis::X];
	FVector3 mLastHorizonSightDir = FVector3::Axis[EAxis::X];

protected:
	std::weak_ptr<CCollider> mMoveTestCollider;

protected:
	std::shared_ptr<FSharedGameState> mSharedState;
	std::unordered_map<std::string, std::vector<std::shared_ptr<CCharacterMovement>>> mMovements;

protected:
	std::unordered_map<std::string, int32> mActiveMovements;
	std::unordered_map<std::string, int32> mLockMovementTags;
};

