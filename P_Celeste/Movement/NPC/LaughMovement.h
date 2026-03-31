#pragma once

#include "Movement/CharacterMovement.h"

class CWidgetComponent;

class CLaughMovement : public CCharacterMovement
{
public:
	CLaughMovement() :
		CCharacterMovement("Laugh", EMovementRecallRule::Ignore)
	{
	}

protected:
	virtual EMovementResult Move(const void* Payload) override;
	virtual void Stop() override;

private:
	std::weak_ptr<CWidgetComponent> mLaughWidgetComp;
};

