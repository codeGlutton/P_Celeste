#include "Pch.h"
#include "AnimStateMachine.h"

#include "Component/Animation2DComponent.h"

void CAnimStateMachine::SetTargetComponent(const std::shared_ptr<CAnimation2DComponent>& Comp)
{
	mTargetComp = Comp;
}

void CAnimStateMachine::CreateState(const std::string& Name, std::function<FAnimStateData()> OnApply, bool IsStartState)
{
	ASSERT_MSG(OnApply != nullptr, "Anim state must need apply event");

	if (mStates.find(Name) == mStates.end())
	{
		mStates.insert({ Name, FAnimState(OnApply)});
	}

	if (IsStartState == true)
	{
		ForcedTransition(Name);
	}
}

void CAnimStateMachine::AddTransition(const std::string& StartState, const std::string& EndState, std::function<bool(std::shared_ptr<const CAnimation2DComponent>&, float)> Codition, int32 Priority, std::function<bool()> OnStart)
{
	if (mStates.find(StartState) == mStates.end() || mStates.find(EndState) == mStates.end())
	{
		return;
	}
	mSortedTransitions.emplace_back(
		std::unique_ptr<FAnimTransition>(new FAnimTransition(&mStates[EndState], Codition, Priority, OnStart))
	);

	FAnimTransition* NewTransition = mSortedTransitions.back().get();
	std::list<FAnimTransition*>& ExisitedTransitions = mStates[StartState].mSortedTransitions;
	
	auto Iter = ExisitedTransitions.cbegin();
	for (; Iter != ExisitedTransitions.cend(); ++Iter)
	{
		if ((*Iter)->GetPriority() < NewTransition->GetPriority())
		{
			break;
		}
	}
	ExisitedTransitions.insert(Iter, NewTransition);
}

void CAnimStateMachine::AddTransition(const std::string& StartState, const std::string& EndState, int32 Priority, std::function<bool()> OnStart)
{
	AddTransition(StartState, EndState, FAnimTransition::mAutoTransitionCondition, Priority, OnStart);
}

void CAnimStateMachine::ForcedTransition(const std::string& StateName)
{
	std::shared_ptr<CAnimation2DComponent> Comp = mTargetComp.lock();
	if (Comp == nullptr)
	{
		return;
	}

	if (mStates.find(StateName) == mStates.end())
	{
		return;
	}

	mCurState = &mStates[StateName];

	FAnimStateData StateData = mCurState->mOnApplyState();
	Comp->SetPlayTime(StateData.mAnimName, StateData.mOption.mPlayTime);
	Comp->SetPlayRate(StateData.mAnimName, StateData.mOption.mPlayRate);
	Comp->SetLoop(StateData.mAnimName, StateData.mOption.mLoop);
	Comp->SetReverse(StateData.mAnimName, StateData.mOption.mReverse);
	Comp->SetSymmetry(StateData.mAnimName, StateData.mOption.mSymmetry);
	Comp->ChangeAnimation(StateData.mAnimName);
	mLastAnimSymmetry = StateData.mOption.mSymmetry;
	mSymmetryOnSight = StateData.mOption.mSymmetryOnSight;

	for (auto& Event : mOnChangeState)
	{
		Event(StateData.mAnimName);
	}
}

void CAnimStateMachine::ForcedTransition(const std::string& StateName, const FAnimSequenceOptionOverride& Option)
{
	std::shared_ptr<CAnimation2DComponent> Comp = mTargetComp.lock();
	if (Comp == nullptr)
	{
		return;
	}

	if (mStates.find(StateName) == mStates.end())
	{
		return;
	}

	mCurState = &mStates[StateName];

	FAnimStateData StateData = mCurState->mOnApplyState();
	Comp->SetPlayTime(StateData.mAnimName, Option.mPlayTime);
	Comp->SetPlayRate(StateData.mAnimName, Option.mPlayRate);
	Comp->SetLoop(StateData.mAnimName, Option.mLoop);
	Comp->SetReverse(StateData.mAnimName, Option.mReverse);
	Comp->SetSymmetry(StateData.mAnimName, Option.mSymmetry);
	Comp->ChangeAnimation(StateData.mAnimName);
	mLastAnimSymmetry = Option.mSymmetry;
	mSymmetryOnSight = Option.mSymmetryOnSight;

	for (auto& Event : mOnChangeState)
	{
		Event(StateData.mAnimName);
	}
}

void CAnimStateMachine::Update(float DeltaTime, const FVector3& SightDir)
{
	std::shared_ptr<CAnimation2DComponent> Comp = mTargetComp.lock();
	if (Comp == nullptr)
	{
		return;
	}

	if (mCurState == nullptr)
	{
		mCurState = &mStates.begin()->second;
	}

	mStateAccTime += DeltaTime;

	std::shared_ptr<const CAnimation2DComponent> ConstComp = Comp;
	for (FAnimTransition*& Transition : mCurState->mSortedTransitions)
	{
		if (Transition->mCondition(ConstComp, DeltaTime) == true)
		{
			mStateAccTime = 0.f;

			if (Transition->mOnStart != nullptr)
			{
				Transition->mOnStart();
			}

			mCurState = Transition->mTargetState;

			FAnimStateData StateData = mCurState->mOnApplyState();
			Comp->SetPlayTime(StateData.mAnimName, StateData.mOption.mPlayTime);
			Comp->SetPlayRate(StateData.mAnimName, StateData.mOption.mPlayRate);
			Comp->SetLoop(StateData.mAnimName, StateData.mOption.mLoop);
			Comp->SetReverse(StateData.mAnimName, StateData.mOption.mReverse);
			Comp->SetSymmetry(StateData.mAnimName, StateData.mOption.mSymmetry);
			Comp->ChangeAnimation(StateData.mAnimName);
			mLastAnimSymmetry = StateData.mOption.mSymmetry;
			mSymmetryOnSight = StateData.mOption.mSymmetryOnSight;

			for (auto& Event : mOnChangeState)
			{
				Event(StateData.mAnimName);
			}
			break;
		}
	}

	if (mSymmetryOnSight == true)
	{
		mLastAnimSymmetry = SightDir.x < 0;
		Comp->SetSymmetry(Comp->GetAnimationSplitName(), mLastAnimSymmetry);
	}
}