#pragma once

#include "Movement/CharacterMovement.h"

class CDialogueWidget;

class CDialogueMovement : public CCharacterMovement
{
public:
	CDialogueMovement() :
		CCharacterMovement("Dialogue", EMovementRecallRule::Ignore)
	{
		mLooseTagsOnMove.push_back("Jump");
		mLooseTagsOnMove.push_back("Walk");
		mLooseTagsOnMove.push_back("Climb");
		mLooseTagsOnMove.push_back("Dangle");
		mLooseTagsOnMove.push_back("Dash");
		mLooseTagsOnMove.push_back("FastFall");
		mLooseTagsOnMove.push_back("Slide");

		mForbiddenTagsOnMove.push_back("Dead");
	}

protected:
	virtual void Init(std::weak_ptr<CCharacterMovementComponent> Owner) override;
	virtual bool CanMove() const override;
	virtual EMovementResult Move(const void* Payload) override;
	virtual EMovementResult Update(float DeltaTime) override;
	virtual void Stop() override;

protected:
	std::shared_ptr<CDialogueWidget> mWidget;

protected:
	bool mNeedToStop = false;
	float mAccDelayTime = 0.f;
	float mEndDelayTime = 2.f;
};


