#pragma once

#include "Movement/CharacterMovement.h"

class CDashMovement : public CCharacterMovement
{
public:
	CDashMovement() :
		CCharacterMovement("Dash", EMovementRecallRule::Restart),
		mDashPoint(mDashMaxPoint)
	{
		mLooseTagsOnMove.push_back("Jump");
		mLooseTagsOnMove.push_back("Slide");
		mLooseTagsOnMove.push_back("Walk");
		mLooseTagsOnMove.push_back("Dangle");
		mLooseTagsOnMove.push_back("Climb");
		mLooseTagsOnMove.push_back("FastFall");

		mLockTagsOnUpdate.push_back("Slide");
		mLockTagsOnUpdate.push_back("Walk");
		mLockTagsOnUpdate.push_back("Dangle");
		mLockTagsOnUpdate.push_back("Climb");
		mLockTagsOnUpdate.push_back("FastFall");
	}

protected:
	virtual void Init(std::weak_ptr<CCharacterMovementComponent> Owner) override;
	virtual bool CanMove() const override;
	virtual EMovementResult Update(float DeltaTime) override;
	virtual EMovementResult Move(const void* Payload) override;
	virtual void Stop() override;
	virtual void Finish() override;

public:
	void SetDashDistance(float Distance)
	{
		mDashDistance = Distance;
	}
	void SetDashMaxTime(float MaxTime)
	{
		mDashMaxTime = MaxTime;
	}

private:
	void ApplyGravity(bool UseGravity);

public:
	void SpendPoint();
	void RechargeAllPoint();
	void RechargePoint(int32 Point);

public:
	std::vector<std::function<void(int32 /* CurrentPoint */, int32 /* ChangedPoint */)>> mOnSpendPoint;
	std::vector<std::function<void(int32 /* CurrentPoint */, int32 /* ChangedPoint */)>> mOnRechargePoint;

protected:
	float mDashDistance = 160.f;

protected:
	int32 mDashMaxPoint = 1;
	int32 mDashPoint;

protected:
	float mDashMaxTime = 0.21f;
	float mDashTime = 0.f;

private:
	bool mIsSweep = false;
	FVector3 mPrePos = FVector3::Zero;
	FVector3 mPreInterpPos = FVector3::Zero;

	FVector3 mMidInterpPos = FVector3::Zero;
	FVector3 mEndInterpPos = FVector3::Zero;
	float mPreUseMaxDropVelocity;
};

