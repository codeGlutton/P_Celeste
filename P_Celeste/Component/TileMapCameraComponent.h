#pragma once

#include "Component/CameraComponent.h"

class CTileMapCameraComponent : public CCameraComponent
{
	friend class CGameObject;
	friend class CObject;

protected:
	CTileMapCameraComponent() = default;
	CTileMapCameraComponent(const CTileMapCameraComponent& ref) = default;
	CTileMapCameraComponent(CTileMapCameraComponent&& ref)	noexcept = default;

public:
	virtual ~CTileMapCameraComponent() = default;

public:
	virtual bool Init() override;
	virtual void UpdateOnMain(float DeltaTime) override;
	virtual void OnChangeToMainCamera() override;

public:
	FVector3 GetClampCameraPos() const;

protected:
	virtual CTileMapCameraComponent* Clone() const;

public:
	void SetDesiredRelativePos(const FVector3& Pos)
	{
		mDesiredRelativePos = Pos;
	}
	const FVector3& GetDesiredRelativePos() const
	{
		return mDesiredRelativePos;
	}

protected:
	FVector3 mPreClampCameraPos = FVector3::Zero;
	FVector3 mDesiredRelativePos = FVector3::Zero;
	static constexpr float mClampSmoothRate = 0.25f;
};

