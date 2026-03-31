#pragma once

#include "MovementComponent.h"
#include "../World/NavAgent.h"

class CObjectMovementComponent :
	public CMovementComponent,
	public CNavAgent
{
	friend class CGameObject;

protected:
	CObjectMovementComponent();
	CObjectMovementComponent(const CObjectMovementComponent& ref);
	CObjectMovementComponent(CObjectMovementComponent&& ref)	noexcept;

public:
	virtual ~CObjectMovementComponent();

public:
	const FVector3& GetVelocity()	const
	{
		return mVelocity;
	}

	const FVector3& GetMoveDir()	const
	{
		return mMoveDir;
	}

	const FVector3& GetLastMoveDir()	const
	{
		return mLastMoveDir;
	}

	float GetSpeed()	const
	{
		return mSpeed;
	}

	float GetCurSpeed()	const
	{
		return mCurSpeed;
	}

	float GetAccel()	const
	{
		return mAccel;
	}

	float GetVelocityFrame()	const
	{
		if (mMoveDir.IsZero())
			return 0.f;

		return (mCurSpeed + mAccel * CTimer::GetDeltaTime()) * CTimer::GetDeltaTime();
	}

	/* 누적형 이동 */
public:
	void SetSpeed(float Speed)
	{
		mSpeed = Speed;
	}

	void SetAccel(float Accel)
	{
		mAccel = Accel;
	}

	void AddMove(const FVector3& Dir)
	{
		if (mApplyMove == false)
		{
			return;
		}

		mMoveDir += Dir;
	}

	void SetApplyMove(bool CanApply)
	{
		mApplyMove = CanApply;
	}

public:
	void MoveStraight(const FVector3& Pos);

	/* 경로형 이동 (단순 직선) */
public:
	void Move(const FVector3& Pos);
	void Move(const FVector2& Pos);
	void MovePath(const FVector3& Pos);
	void MovePath(const FVector2& Pos);
	virtual void StartPath();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PostRender();
	virtual void Destroy();

protected:
	virtual CObjectMovementComponent* Clone()	const;

protected:
	FVector3		mLastMoveDir = FVector3::Zero;
	FVector3		mMoveDir = FVector3::Zero;
	float			mSpeed = 200.f;
	float			mCurSpeed = 0.f;
	float			mAccel = 2500.f;
	FVector3		mVelocity;
	bool			mApplyMove = true;
};

