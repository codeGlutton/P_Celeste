#pragma once

#include "Component/CameraComponent.h"

class CCutsceenCameraComponent : public CCameraComponent
{
	friend class CGameObject;
	friend class CObject;

protected:
	CCutsceenCameraComponent() = default;
	CCutsceenCameraComponent(const CCutsceenCameraComponent& ref) = default;
	CCutsceenCameraComponent(CCutsceenCameraComponent&& ref)	noexcept = default;

public:
	virtual ~CCutsceenCameraComponent() = default;

public:
	virtual void UpdateOnMain(float DeltaTime) override;

protected:
	virtual CCutsceenCameraComponent* Clone() const;

public:
	void SetTargetZoomRate(float ZoomRate)
	{
		mIsMoving = true;
		mCutsceenAccTime = 0.f;
		mTargetZoomRate = ZoomRate;
	}
	void SetMoveRelativePos(const FVector3& Pos)
	{
		mIsMoving = true;
		mCutsceenAccTime = 0.f;
		mMoveRelativePos = Pos;
	}

public:
	std::function<void()> OnEndCutsceen;

protected:
	bool mIsMoving = false;
	float mTargetZoomRate = 1.f;
	FVector3 mMoveRelativePos = FVector3::Zero;

protected:
	static constexpr float mCutsceenMaxTime = 2.f;
	float mCutsceenAccTime = 0.f;
};

