#pragma once

#include "../EngineInfo.h"
#include "ThreadNavigation.h"

class CWorldNavigation
{
	friend class CWorld;

private:
	CWorldNavigation();

public:
	~CWorldNavigation();

public:
	void AddData(int Header, int Size, unsigned char* Data);

private:
	void Begin();

public:
	bool Init();
	/**
	 * 내비 스레드 작업 결과를 큐에서 빼옴
	 * \param DeltaTime
	 */
	void Update(float DeltaTime);
	/**
	 * 내비 스레드 만들어두기
	 * \param Count 스레드 갯수
	 * \param TileMap 타겟 타일맵
	 */
	void CreateNavigationThread(int Count,
		const std::weak_ptr<class CTileMapComponent>& TileMap);
	/**
	 * 내비 스레드에 일거리 넘기기
	 * \param Start 시작점
	 * \param End 도착점
	 * \param Agent 명령 컴포넌트
	 */
	void FindPath(const FVector3& Start, const FVector3& End,
		std::weak_ptr<class CComponent>* Agent);

private:
	/**
	 * 내비 스레드가 종료됨을 탐지시 실행
	 * \param Size 결과 사이즈
	 * \param Data 결과 데이터
	 */
	void NavigationComplete(int Size, unsigned char* Data);

private:
	std::weak_ptr<class CWorld>	mWorld;
	std::weak_ptr<CWorldNavigation>	mSelf;

	// 내비 스레드
	std::vector<std::shared_ptr<CThreadNavigation>>	mThreadList;
	// 내비 스레드와 게임 스레드가 공유하는 큐
	std::shared_ptr<CThreadQueue>	mNavQueue;
};

