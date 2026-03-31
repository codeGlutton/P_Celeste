#include "MovementComponent.h"
#include "SceneComponent.h"

CMovementComponent::CMovementComponent()
{
}

CMovementComponent::CMovementComponent(
	const CMovementComponent& ref)	:
	CObjectComponent(ref)
{
}

CMovementComponent::CMovementComponent(
	CMovementComponent&& ref) noexcept :
	CObjectComponent(std::move(ref))
{
}

CMovementComponent::~CMovementComponent()
{
}

void CMovementComponent::SetUpdateComponent(
	const std::weak_ptr<CSceneComponent>& Component)
{
	mUpdateComponent = Component;
}

const std::weak_ptr<CSceneComponent>& CMovementComponent::GetUpdateComponent()
{
	return mUpdateComponent;
}

bool CMovementComponent::Init()
{
	return true;
}

void CMovementComponent::Update(float DeltaTime)
{
}

void CMovementComponent::PostUpdate(float DeltaTime)
{
}

void CMovementComponent::PostRender()
{
	CObjectComponent::PostRender();
}

void CMovementComponent::Destroy()
{
	CObjectComponent::Destroy();
}
