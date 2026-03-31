#include "Pch.h"
#include "SpringArmComponent.h"

CSpringArmComponent::CSpringArmComponent()
{
	SetClassType<CSpringArmComponent>();
}

bool CSpringArmComponent::Init()
{
	CSceneComponent::Init();
	mPrePos = GetWorldPos();

	return true;
}

void CSpringArmComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);

	std::shared_ptr<CSceneComponent> ParentComp = mParent.lock();
	if (ParentComp == nullptr)
	{
		return;
	}

	const FVector3& DesiredPos = ParentComp->GetWorldPos() + mDesiredRelativePos;

	float Alpha = 1.0f - std::exp(-mLerpRate * DeltaTime);
	FVector3 NextPos = MathUtils::Lerp(mPrePos, DesiredPos, Alpha);

	FVector3 Dir = NextPos - DesiredPos;
	float Dist = Dir.Length();
	Dir.Normalize();
	if (Dist > mMaxDist)
	{
		NextPos = DesiredPos + Dir * mMaxDist;
	}

	SetWorldPos(NextPos);
	mPrePos = NextPos;
}
