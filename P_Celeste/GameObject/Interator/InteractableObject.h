#pragma once

#include "Object/GameObject.h"
#include "Movement/SharedGameState.h"

class CCharacterMovement;

class CInteractableObject : public CGameObject
{
public:
	virtual void OnTouchedDown(ETouchedWall::Type Type, std::shared_ptr<CGameObject> Interactor)
	{
	}
	virtual void OnTouchedUp(ETouchedWall::Type Type, std::shared_ptr<CGameObject> Interactor)
	{
	}
	virtual void OnLanded(std::shared_ptr<CGameObject> Interactor)
	{
	}
	virtual void OnJumped(std::shared_ptr<CGameObject> Interactor)
	{
	}
	virtual void OnEffectedByMovement(std::shared_ptr<CGameObject> Interactor, std::shared_ptr<CCharacterMovement> Movement)
	{
	}
	virtual void OnFinishedByMovement(std::shared_ptr<CGameObject> Interactor, std::shared_ptr<CCharacterMovement> Movement)
	{
	}
};

