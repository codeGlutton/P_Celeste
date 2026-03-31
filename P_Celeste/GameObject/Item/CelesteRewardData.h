#pragma once

#include "Object/RewardManager.h"

class CCelesteRewardData : public IRewardData
{
public:
	virtual void Save(FILE* File) const override;
	virtual void Load(FILE* File) override;

public:
	std::unordered_set<int32> mStrawberryCollection;
};

