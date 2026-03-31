#pragma once

#include "../EngineInfo.h"

class IRewardData
{
public:
	virtual ~IRewardData() = default;

public:
	virtual void Save(FILE* File) const = 0;
	virtual void Load(FILE* File) = 0;
};

class CRewardManager
{
public:
	static CRewardManager* GetInst()
	{
		if (!mInst)
			mInst = new CRewardManager;
		return mInst;
	}

	static void DestroyInst()
	{
		mInst->Save();
		SAFE_DELETE(mInst);
	}

public:
	bool Init()
	{
		Load();
		return true;
	}

public:
	template<typename T>
	void MakeData()
	{
		mRewardData = std::make_shared<T>();
	}
	template<typename T = IRewardData>
	std::shared_ptr<T> GetData()
	{
		return std::static_pointer_cast<T>(mRewardData);
	}

protected:
	void Save() const;
	void Load();

private:
	static CRewardManager* mInst;

private:
	std::shared_ptr<IRewardData> mRewardData;
};

