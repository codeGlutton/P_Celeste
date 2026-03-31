#pragma once

#include "Movement/CharacterMovement.h"

class CCharacterMovementComponent;

class CDangleMovement : public CCharacterMovement
{
public:
	CDangleMovement() :
		CCharacterMovement("Dangle", EMovementRecallRule::Ignore)
	{
		mLooseTagsOnMove.push_back("Jump");
		mLooseTagsOnMove.push_back("Walk");
		mLooseTagsOnMove.push_back("FastFall");

		mLockTagsOnUpdate.push_back("Walk");
		mLockTagsOnUpdate.push_back("FastFall");
	}

protected:
	virtual void Init(std::weak_ptr<CCharacterMovementComponent> Owner) override;
	virtual bool CanMove() const override;
	virtual EMovementResult Update(float DeltaTime) override;
	virtual EMovementResult Move(const void* Payload) override;
	virtual void Stop() override;

public:
	const FVector3& GetDangleDir() const
	{
		return mDangleDir;
	}

public:
	void RechargeAllPoint();

private:
	void ApplyGravity(bool UseGravity);

public:
	std::vector<std::function<void(float /* ChangedTime */)>> mOnWarnLimit;
	std::vector<std::function<void(float /* ChangedTime */)>> mOnRechargePoint;

protected:
	FVector3 mDangleDir = FVector3::Zero;

	float mDangleMaxTime = 12.5f;
	float mDangleWarningTime = 10.f;
	float mDangleSpendTime = 2.5f;
	float mDangleTime = 0.f;

	bool mWasWarn = false;

protected:
	float mDangleSpeedRate = 0.5f;
	float mPreSpeed = 0.f;

protected:
	float mReleaseJumpSpeed = 400.f;

protected:
	std::shared_ptr<CMovementEffector> mEffector = nullptr;
};

