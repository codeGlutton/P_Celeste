#pragma once

#include "Animation/AnimState.h"
#include "Animation/AnimTransition.h"

class CAnimation2DComponent;

class CAnimStateMachine
{
public:
	CAnimStateMachine() = default;

public:
	void SetTargetComponent(const std::shared_ptr<CAnimation2DComponent>& Comp);

public:
	void CreateState(
		const std::string& Name,
		std::function<FAnimStateData()> OnApply,
		bool IsStartState = false
	);
	void AddTransition(
		const std::string& StartState,
		const std::string& EndState,
		std::function<bool(std::shared_ptr<const CAnimation2DComponent>&, float)> Codition = FAnimTransition::mAutoTransitionCondition,
		int32 Priority = 0,
		std::function<bool()> OnStart = nullptr
	);
	void AddTransition(
		const std::string& StartState,
		const std::string& EndState,
		int32 Priority,
		std::function<bool()> OnStart = nullptr
	);
	void ForcedTransition(const std::string& StateName);
	void ForcedTransition(const std::string& StateName, const FAnimSequenceOptionOverride& Option);

public:
	void Update(float DeltaTime, const FVector3& SightDir);

public:
	float GetCurStateAccTime() const
	{
		return mStateAccTime;
	}
	bool GetLastAnimSymmetry() const
	{
		return mLastAnimSymmetry;
	}

public:
	std::vector<std::function<void(const std::string&)>> mOnChangeState;

private:
	std::weak_ptr<CAnimation2DComponent> mTargetComp;

private:
	FAnimState* mCurState = nullptr;
	std::unordered_map<std::string, FAnimState> mStates;
	std::vector<std::unique_ptr<FAnimTransition>> mSortedTransitions;

private:
	float mStateAccTime = 0.f;
	bool mSymmetryOnSight = true;
	bool mLastAnimSymmetry = false;
};

