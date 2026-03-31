#pragma once

#include "GameObject/Interator/InteractableObject.h"

class CPlayerInteractableObject : public CInteractableObject
{
public:
	virtual void OnEffectedByMovement(std::shared_ptr<CGameObject> Interator, std::shared_ptr<CCharacterMovement> Movement) override;
	virtual void OnFinishedByMovement(std::shared_ptr<CGameObject> Interator, std::shared_ptr<CCharacterMovement> Movement) override;
	virtual void OnGraped(std::shared_ptr<CGameObject> Interator)
	{
	}
	virtual void OnReleased(std::shared_ptr<CGameObject> Interator)
	{
	}
	virtual void OnDashed(std::shared_ptr<CGameObject> Interator)
	{
	}
};

