#include "Pch.h"
#include "PlayerJumpMovement.h"

#include "Movement/PlayerGameState.h"

bool CPlayerJumpMovement::CanMove() const
{
	if (CJumpMovement::CanMove() == true)
	{
		return true;
	}
	FPlayerGameState* PlayerState = static_cast<FPlayerGameState*>(GetSharedGameState());
	FSharedGameState* SharedState = GetSharedGameState();
	if (PlayerState != nullptr && PlayerState->GetAirTime() <= mAirJumpMaxTime && SharedState->IsSideOfWall() == false)
	{
		return true;
	}
	return false;
}
