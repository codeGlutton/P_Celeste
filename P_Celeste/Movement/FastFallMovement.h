#pragma once

#include "Movement/CharacterMovement.h"

class CFastFallMovement : public CCharacterMovement
{
public:
	CFastFallMovement() :
		CCharacterMovement("FastFall", EMovementRecallRule::Ignore)
	{
	}

protected:
	virtual bool CanMove() const override;
	virtual EMovementResult Move(const void* Payload) override;
	virtual EMovementResult Update(float DeltaTime) override;
	virtual void Stop() override;

protected:
	bool CanFall() const;

protected:
	float mMaxDropRate = 1.5f;
	float mPreMaxDropSpeed = 0.f;
};

