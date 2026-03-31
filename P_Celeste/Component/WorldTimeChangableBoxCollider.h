#pragma once

#include "Component/ColliderBox2D.h"

class CWorldTimeChangableBoxCollider : public CColliderBox2D
{
	friend class CGameObject;
	friend class CObject;

protected:
	CWorldTimeChangableBoxCollider();
	CWorldTimeChangableBoxCollider(const CWorldTimeChangableBoxCollider& ref) = default;
	CWorldTimeChangableBoxCollider(CWorldTimeChangableBoxCollider&& ref) noexcept = default;

public:
	virtual ~CWorldTimeChangableBoxCollider() override = default;

protected:
	virtual CWorldTimeChangableBoxCollider* Clone()	const override;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

public:
	void SetOneTimeTrigger(bool IsOneTimeTrigger)
	{
		mIsOneTimeTrigger = IsOneTimeTrigger;
	}
	void ResetTrigger();

public:
	void SetTargetWorldTimeRate(float TargetRate)
	{
		mTargetWorldTimeRate = TargetRate;
	}
	void SetWorldTimeChangeMaxTime(float MaxTime)
	{
		mWorldTimeChangeMaxTime = MaxTime;
	}
	void ResetWorldTimeRate();

public:
	std::vector<std::function<void()>> mOnStartLerpWorldTime;
	std::vector<std::function<void()>> mOnChangeTargetWorldTime;

protected:
	bool mIsOneTimeTrigger = false;
	bool mIsTrigger = false;

protected:
	bool mIsLerpRate = false;
	float mTargetWorldTimeRate = 1.f;
	float mWorldTimeChangeMaxTime = 0.f;
	float mWorldTimeChangeAccTime = 0.f;
};

