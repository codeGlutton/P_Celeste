#include "Pch.h"
#include "PlayerInteractableObject.h"

#include "Movement/CharacterMovement.h"

void CPlayerInteractableObject::OnEffectedByMovement(std::shared_ptr<CGameObject> Interator, std::shared_ptr<CCharacterMovement> Movement)
{
	CInteractableObject::OnEffectedByMovement(Interator, Movement);
	if (Movement->GetName() == "Dangle")
	{
		OnGraped(Interator);
	}
	else if (Movement->GetName() == "Dash")
	{
		OnDashed(Interator);
	}
}

void CPlayerInteractableObject::OnFinishedByMovement(std::shared_ptr<CGameObject> Interator, std::shared_ptr<CCharacterMovement> Movement)
{
	CInteractableObject::OnFinishedByMovement(Interator, Movement);
	if (Movement->GetName() == "Dangle")
	{
		OnReleased(Interator);
	}
}
