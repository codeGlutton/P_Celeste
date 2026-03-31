#pragma once

#include "Movement/CharacterMovement.h"

class CCharacterMovementComponent;

class CClimbMovement : public CCharacterMovement
{
public:
	CClimbMovement() :
		CCharacterMovement("Climb", EMovementRecallRule::Ignore)
	{
		mRequiredTagsOnMove.push_back("Dangle");
	}

protected:
	virtual bool CanMove() const override;
	virtual EMovementResult Move(const void* Payload) override;
};

