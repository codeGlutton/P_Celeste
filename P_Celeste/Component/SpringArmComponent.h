#pragma once

#include "Component/SceneComponent.h"

class CSpringArmComponent : public CSceneComponent
{
	friend class CGameObject;
	friend class CObject;

protected:
	CSpringArmComponent();
	CSpringArmComponent(const CSpringArmComponent& ref) = default;
	CSpringArmComponent(CSpringArmComponent&& ref) noexcept = default;

public:
	virtual ~CSpringArmComponent() override = default;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

public:
	void SetMaxDist(float Max)
	{
		mMaxDist = Max;
	}
	void SetDesiredRelativePos(const FVector3& Pos)
	{
		mPrePos = GetWorldPos() + Pos;
		mDesiredRelativePos = Pos;
	}

protected:
	float mMaxDist = 250.f;

protected:
	static constexpr float mLerpRate = 5.f;

private:
	FVector3 mPrePos;
	FVector3 mDesiredRelativePos = FVector3::Zero;
};

