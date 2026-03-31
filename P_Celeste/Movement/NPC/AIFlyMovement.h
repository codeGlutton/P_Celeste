#pragma once

#include "Movement/CharacterMovement.h"

class CCharacterMovementComponent;

class CAIFlyMovement : public CCharacterMovement
{
public:
	CAIFlyMovement() :
		CCharacterMovement("AIFly", EMovementRecallRule::Ignore)
	{
	}

protected:
	virtual bool CanMove() const override;
	virtual EMovementResult Move(const void* Payload) override;
	virtual EMovementResult Update(float DeltaTime) override;

public:
	std::vector<std::function<void(const FVector3& /* MoveDir */)>> mOnFly;

private:
	FVector3 mDestPos;
	FVector3 mMoveDir;

private:
	float mDistThreshold = 10.f;
	bool mFollowTargetPos = false;
};


