#pragma once

#include "EngineInfo.h"
#include "ThreadQueue.h"
#include "Sync.h"
#include <thread>
#include <atomic>

class CThreadBase abstract
{
	friend class CThreadManager;

protected:
	CThreadBase();

public:
	virtual ~CThreadBase();

public:
	bool GetComplete()
	{
		CSync   sync(&mCrt);

		return mComplete;
	}

public:
	void SetName(const std::string& Name)
	{
		mName = Name;
	}

	const std::string& GetName()	const
	{
		return mName;
	}

public:
	bool Init(bool Pause = false);
	virtual void Exit();
	virtual void Run() = 0;

public:
	void AddData(int Header, int Size, unsigned char* Data);
	void GetData(int& Header, int& Size, unsigned char* Data);
	int GetQueueSize();
	bool EmptyQueue();

public:
	void Suspend();
	void Resume();

private:
	static unsigned int __stdcall ThreadFunc(void* Arg);

protected:
	// 스레드를 찾기 위한 이름
	std::string		mName;
	HANDLE			mThread = nullptr;
	std::shared_ptr<CThreadQueue>	mQueue;
	std::atomic<bool>			mLoop = false;
	bool			mComplete = false;
	// 스레드 접근에 대한 크리티컬 섹션
	CRITICAL_SECTION	mCrt;
};

