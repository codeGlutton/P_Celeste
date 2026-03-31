#include "Pch.h"
#include "AnimTransition.h"

#include "Component/Animation2DComponent.h"

const std::function<bool(std::shared_ptr<const CAnimation2DComponent>&, float)> FAnimTransition::mAutoTransitionCondition = [](std::shared_ptr<const CAnimation2DComponent>& Comp, float DeltaTime) -> bool {
	std::shared_ptr<const CAnimation2DSequence> Sequence = Comp->GetAnimationSequence();
	if (Sequence == nullptr || Sequence->GetAsset().expired() == true || Sequence->IsLoop() == true)
	{
		return false;
	}

	return Sequence->IsEnd();
	};

FAnimTransition::FAnimTransition(FAnimState* TargetState, std::function<bool(std::shared_ptr<const CAnimation2DComponent>&, float)> Condition, int32 Priority, std::function<bool()> OnStart) :
	mTargetState(TargetState),
	mCondition(std::move(Condition)),
	mPriority(Priority),
	mOnStart(std::move(OnStart))
{
}

int32 FAnimTransition::GetPriority() const
{
	return mPriority;
}

