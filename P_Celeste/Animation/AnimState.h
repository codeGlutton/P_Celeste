#pragma once

struct FAnimTransition;

struct FAnimSequenceOptionOverride
{
	float mPlayTime = 1.f;
	float mPlayRate = 1.f;
	bool mLoop = false;
	bool mReverse = false;
	bool mSymmetry = false;
	bool mSymmetryOnSight = true;
};

struct FAnimStateData
{
	const std::string mAnimName;
	FAnimSequenceOptionOverride mOption;
};

struct FAnimState
{
public:
	FAnimState() = default;
	FAnimState(std::function<FAnimStateData()> OnApplyState) :
		mOnApplyState(OnApplyState)
	{
	}

public:
	std::function<FAnimStateData()> mOnApplyState;
	std::list<FAnimTransition*> mSortedTransitions;
};

