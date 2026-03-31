#pragma once

#include "ObjectComponent.h"
#include "../Timer.h"

class CSceneComponent;

class CMovementComponent :
    public CObjectComponent
{
	friend class CGameObject;

protected:
	CMovementComponent();
	CMovementComponent(const CMovementComponent& ref);
	CMovementComponent(CMovementComponent&& ref)	noexcept;

public:
	virtual ~CMovementComponent();

public:
	void SetUpdateComponent(const std::weak_ptr<CSceneComponent>& Component);
	const std::weak_ptr<CSceneComponent>& GetUpdateComponent();

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PostRender();
	virtual void Destroy();

protected:
	virtual CMovementComponent* Clone()	const = 0;

protected:
	std::weak_ptr<CSceneComponent>	mUpdateComponent;
};

