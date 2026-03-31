#pragma once

#include "SceneComponent.h"

enum class ECameraProjectionType : unsigned char
{
	Perspective,
	Ortho
};

class CCameraComponent :
    public CSceneComponent
{
	friend class CGameObject;
	friend class CObject;

protected:
	CCameraComponent();
	CCameraComponent(const CCameraComponent& ref);
	CCameraComponent(CCameraComponent&& ref)	noexcept;

public:
	virtual ~CCameraComponent();

public:
	const FMatrix& GetViewMatrix()	const
	{
		return mViewMatrix;
	}

	const FMatrix& GetProjMatrix()	const
	{
		return mProjMatrix;
	}

	float GetZoomRate()	const
	{
		return mZoomRate;
	}

	void SetProjection(ECameraProjectionType Type, float ViewAngle, float Width, float Height, float ViewDistance);
	void SetZoomRate(float ZoomRate);
	void Zoom(float DeltaRate);

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void UpdateOnMain(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);

public:
	virtual void OnChangeToMainCamera();

protected:
	virtual CCameraComponent* Clone()	const;

protected:
	ECameraProjectionType	mProjType = ECameraProjectionType::Perspective;
	FMatrix	mViewMatrix;
	FMatrix	mProjMatrix;

	float	mViewAngle = 90.f;
	float	mWidth = 1280.f;
	float	mHeight = 720.f;
	float	mViewDistance = 1000.f;

protected:
	float mZoomRate = 1.0f;
};

