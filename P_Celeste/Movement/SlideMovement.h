#pragma once

#include "Movement/CharacterMovement.h"

class CSlideMovement : public CCharacterMovement
{
public:
	CSlideMovement() :
		CCharacterMovement("Slide", EMovementRecallRule::Ignore)
	{
		mForbiddenTagsOnMove.push_back("Dangle");
		mForbiddenTagsOnMove.push_back("Dash");
		mForbiddenTagsOnUpdate.push_back("Dangle");
		mForbiddenTagsOnUpdate.push_back("Dash");

		mLooseTagsOnMove.push_back("FastFall");
		mLockTagsOnUpdate.push_back("FastFall");
	}

protected:
	virtual bool CanMove() const override;
	virtual EMovementResult Move(const void* Payload) override;
	virtual EMovementResult Update(float DeltaTime) override;
	virtual void Stop() override;

protected:
	bool CanSlide() const;

public:
	std::vector<std::function<void(bool /*IsMinVelocity*/ )>> mOnApplyFriction;

protected:
	bool mIsInterpedToStart = false;
	float mSlideStartFrictionAccel = 6000.f;
	float mSlideMaxStartVelocity = 50.f;

	float mSlideFrictionAccel = 800.f;
	float mSlideMinDropVelocity = 600.f;
};

