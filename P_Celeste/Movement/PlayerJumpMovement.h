#pragma once

#include "Movement/JumpMovement.h"

class CPlayerJumpMovement : public CJumpMovement
{
protected:
	virtual bool CanMove() const override;

protected:
	float mAirJumpMaxTime = 0.12f;
};

