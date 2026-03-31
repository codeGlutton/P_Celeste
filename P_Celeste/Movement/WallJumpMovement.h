#pragma once

#include "Movement/JumpMovement.h"

class CWallJumpMovement : public CJumpMovement
{
public:
	CWallJumpMovement() :
		CJumpMovement()
	{
	}

protected:
	virtual bool CanMove() const override;
	virtual EMovementResult Move(const void* Payload) override;
	virtual EMovementResult Update(float DeltaTime) override;
	virtual bool CanStop(EMovementStopReason Reason) const override;
	virtual void Stop() override;

private:
	void OnStartWallJump();
	void OnEndWallJump();

public:
	std::vector<std::function<void(const FVector3& /* WallJumpDir */, bool /* NeedToReverseSight */)>> mOnStartWallJump;

private:
	FVector3 mWallJumpDir = FVector3::Zero;
	bool mNeedToReverseSight = false;;
	float mReverseMoveApplyTime = 0.f;
};

