#pragma once

#include "Movement/CharacterMovement.h"

class CDeadMovement : public CCharacterMovement
{
public:
	CDeadMovement() :
		CCharacterMovement("Dead", EMovementRecallRule::Ignore)
	{
		mLooseTagsOnMove.push_back("Jump");
		mLooseTagsOnMove.push_back("Walk");
		mLooseTagsOnMove.push_back("Climb");
		mLooseTagsOnMove.push_back("Dangle");
		mLooseTagsOnMove.push_back("Dash");
		mLooseTagsOnMove.push_back("FastFall");
		mLooseTagsOnMove.push_back("Slide");
	}

protected:
	virtual EMovementResult Move(const void* Payload) override;
	virtual EMovementResult Update(float DeltaTime) override;
	virtual void Stop() override;

public:
	std::vector<std::function<void()>> mOnDead;
	std::vector<std::function<void()>> mOnRevive;

private:
	float mRespawnAccTime = 0.f;
	float mRespawnDelayTime = 0.1f;
};
