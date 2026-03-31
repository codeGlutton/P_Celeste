#include "Pch.h"
#include "WorldTimeChangableBoxCollider.h"

#include "Timer.h"
#include "World/World.h"

CWorldTimeChangableBoxCollider::CWorldTimeChangableBoxCollider()
{
	SetClassType<CWorldTimeChangableBoxCollider>();
}

CWorldTimeChangableBoxCollider* CWorldTimeChangableBoxCollider::Clone() const
{
	return new CWorldTimeChangableBoxCollider(*this);
}

bool CWorldTimeChangableBoxCollider::Init()
{
	CColliderBox2D::Init();

	SetCollisionBeginFunction([this](const FVector3& HitPos, CCollider* Collider) {
		if (mIsTrigger == false)
		{
			mIsTrigger = true;
			mIsLerpRate = true;
			for (auto& Event : mOnStartLerpWorldTime)
			{
				Event();
			}
		}
		});

	return true;
}

void CWorldTimeChangableBoxCollider::Update(float DeltaTime)
{
	CColliderBox2D::Update(DeltaTime);

	if (mIsLerpRate == true)
	{
		mWorldTimeChangeAccTime += CTimer::GetDeltaTime();
		if (mWorldTimeChangeMaxTime <= mWorldTimeChangeAccTime)
		{
			mIsLerpRate = false;
			mWorldTimeChangeAccTime = mWorldTimeChangeMaxTime;

			mWorld.lock()->SetUpdateRate(mTargetWorldTimeRate);
			for (auto& Event : mOnChangeTargetWorldTime)
			{
				Event();
			}

			if (mIsOneTimeTrigger == false)
			{
				ResetTrigger();
			}
		}
		else
		{
			mWorld.lock()->SetUpdateRate(MathUtils::Lerp(
				1.f,
				mTargetWorldTimeRate,
				mWorldTimeChangeAccTime / mWorldTimeChangeMaxTime
			));
		}
	}
}

void CWorldTimeChangableBoxCollider::ResetTrigger()
{
	mIsTrigger = false;
}

void CWorldTimeChangableBoxCollider::ResetWorldTimeRate()
{
	mIsLerpRate = false;
	mWorldTimeChangeAccTime = 0.f;

	mWorld.lock()->SetUpdateRate(1.f);
}
