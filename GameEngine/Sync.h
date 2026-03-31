#pragma once

#include "EngineInfo.h"

/**
 * 크리티컬 섹션 RAII
 */
class CSync
{
public:
	CSync(CRITICAL_SECTION* Crt)
	{
		mCrt = Crt;

		EnterCriticalSection(mCrt);
	}

	~CSync()
	{
		if (mCrt)
			LeaveCriticalSection(mCrt);
	}

private:
	CRITICAL_SECTION* mCrt = nullptr;
};

