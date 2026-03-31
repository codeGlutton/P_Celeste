#pragma once

#include "Character/Player/Player.h"

class CTutorialPlayer : public CPlayer
{
	friend class CWorld;
	friend class CObject;

protected:
	CTutorialPlayer();
	CTutorialPlayer(const CTutorialPlayer& ref) = default;
	CTutorialPlayer(CTutorialPlayer&& ref) noexcept = default;

public:
	virtual ~CTutorialPlayer() override = default;

public:
	virtual bool Init() override;

public:
	void GrantDash(std::function<void()> DashCallback = nullptr)
	{
		mIsGrantDash = true;
		mDashCallback = DashCallback;
	}

private:
	void MoveTutorialDash();

private:
	bool mIsGrantDash = false;
	std::function<void()> mDashCallback;
};

