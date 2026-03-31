#include "Pch.h"
#include "TutorialPlayer.h"

#include "World/World.h"
#include "World/Input.h"

#include "Component/CharacterMovementComponent.h"

CTutorialPlayer::CTutorialPlayer()
{
	SetClassType<CTutorialPlayer>();
}

bool CTutorialPlayer::Init()
{
	CPlayer::Init();

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CInput> InputMgr = World->GetInput().lock();

	InputMgr->SetBindFunction<CTutorialPlayer>("Dash", EInputType::Press, this, &CTutorialPlayer::MoveTutorialDash);

    return true;
}

void CTutorialPlayer::MoveTutorialDash()
{
	if (mIsGrantDash == true)
	{
		mIsGrantDash = false;
		if (mDashCallback != nullptr)
		{
			mDashCallback();
			mDashCallback = nullptr;
		}
		mMovementComponent->DoMovement("Dash");
	}
}
