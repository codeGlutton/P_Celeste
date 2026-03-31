#pragma once

struct FAnimState;
class CAnimation2DComponent;

struct FAnimTransition
{
	friend class CAnimStateMachine;

private:
	FAnimTransition(
		FAnimState* TargetState,
		std::function<bool(std::shared_ptr<const CAnimation2DComponent>&, float)> Condition = mAutoTransitionCondition,
		int32 Priority = 0,
		std::function<bool()> OnStart = nullptr
	);

public:
	int32 GetPriority() const;

private:
	const std::function<bool(std::shared_ptr<const CAnimation2DComponent>&, float)> mCondition;
	const std::function<bool()> mOnStart;

private:
	const int32 mPriority;
	FAnimState* const mTargetState;

private:
	static const std::function<bool(std::shared_ptr<const CAnimation2DComponent>&, float)> mAutoTransitionCondition;
};

