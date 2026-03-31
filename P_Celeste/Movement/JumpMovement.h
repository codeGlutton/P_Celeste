#pragma once

#include "Movement/CharacterMovement.h"

class CJumpMovement : public CCharacterMovement
{
public:
	CJumpMovement() :
		CCharacterMovement("Jump", EMovementRecallRule::Restart)
	{
		mLooseTagsOnMove.push_back("Jump");
		mLooseTagsOnMove.push_back("Dangle");
		mLooseTagsOnMove.push_back("FastFall");

		mLockTagsOnUpdate.push_back("FastFall");
	}

protected:
	virtual bool CanMove() const override;
	virtual EMovementResult Update(float DeltaTime) override;
	virtual EMovementResult Move(const void* Payload) override;
	virtual void Stop() override;

public:
	void SetJumpVelocity(float Velocity)
	{
		mJumpVelocity = Velocity;
	}
	void SetJumpGravityScale(float Scale)
	{
		mJumpGravityScale = Scale;
	}

protected:
	float mJumpVelocity = 440.f;

protected:
	float mJumpGravityScale = 1.f;
	float mPreJumpGravityScale;

protected:
	float mJumpMaxTime = 0.4f;
	float mJumpTime = 0.f;
};

