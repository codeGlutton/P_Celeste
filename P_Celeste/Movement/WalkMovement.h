#pragma once

#include "Movement/CharacterMovement.h"

class CWalkMovement : public CCharacterMovement
{
public:
	CWalkMovement() :
		CCharacterMovement("Walk", EMovementRecallRule::Restart)
	{
		mForbiddenTagsOnMove.push_back("Dangle");
	}

protected:
	virtual bool CanMove() const override;
	virtual EMovementResult Move(const void* Payload) override;
};

