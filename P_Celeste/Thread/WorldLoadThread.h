#pragma once

#include "ThreadBase.h"

class CWorldLoadThread : public CThreadBase
{
	friend class CThreadManager;

protected:
	CWorldLoadThread();

public:
	virtual ~CWorldLoadThread() override;

public:
	void SetWorldType(EWorldType Type)
	{
		mWorldType = Type;
	}

public:
	virtual void Exit() override;
	virtual void Run() override;

private:
	EWorldType mWorldType = EWorldType::None;
};

