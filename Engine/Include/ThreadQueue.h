#pragma once

#include "Sync.h"

struct FThreadQueueData
{
	// 데이터 타입 정보
	int		Header = 0;
	// 실 데이터 사이즈
	int		Size = 0;
	// 버퍼
	unsigned char	Data[1024] = {};
};

class CThreadQueue
{
	friend class CThreadBase;

public:
	CThreadQueue();
	~CThreadQueue();

public:
	void push(int Header, int Size, unsigned char* Data);
	void pop(int& Header, int& Size, unsigned char* Data);
	int size();
	bool full();
	bool empty();

private:
	FThreadQueueData	mData[200];
	int		mPush = 0;
	int		mPop = 0;
	int		mSize = 0;

	// 큐 접근에 대한 크리티컬 섹션
	CRITICAL_SECTION	mCrt;
};

