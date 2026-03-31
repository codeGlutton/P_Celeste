#include "Pch.h"
#include "CutsceenCameraComponent.h"

void CCutsceenCameraComponent::UpdateOnMain(float DeltaTime)
{
	CCameraComponent::UpdateOnMain(DeltaTime);

	if (mIsMoving == true)
	{
		mCutsceenAccTime += DeltaTime;
		if (mCutsceenAccTime >= mCutsceenMaxTime)
		{
			mIsMoving = false;
			mCutsceenAccTime = mCutsceenMaxTime;

			if (OnEndCutsceen != nullptr)
			{
				OnEndCutsceen();
				OnEndCutsceen = nullptr;
			}
		}

		const float LerpRate = mCutsceenAccTime / mCutsceenMaxTime;

		SetRelativePos(MathUtils::Lerp(GetRelativePos(), mMoveRelativePos, LerpRate));
		SetZoomRate(MathUtils::Lerp(GetZoomRate(), mTargetZoomRate, LerpRate));
	}
}

CCutsceenCameraComponent* CCutsceenCameraComponent::Clone() const
{
	return new CCutsceenCameraComponent(*this);
}