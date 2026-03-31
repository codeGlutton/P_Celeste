#pragma once

#include "EngineInfo.h"
#include "Object.h"
#include "EngineSetting.h"

class CEngine
{
private:
	CEngine();
	~CEngine();

public:
	HINSTANCE GetWindowInstance()	const
	{
		return mhInst;
	}

	HWND GetWindowHandle()	const
	{
		return mhWnd;
	}

public:
	/**
	 * 윈도우 생성, 싱글톤 및 세팅 초기화 함수
	 * (디바이스->충돌->에셋->랜더->스레드)
	 */
	bool Init(HINSTANCE hInst, const TCHAR* WindowName, 
		int IconID,
		int SmallIconID, int Width, int Height, 
		bool WindowMode = true);
	int Run();
	void Destroy();

private:
	/**
	 * 가장 바깥에 존재하는 반복 로직
	 * (타이머->월드 업데이트->월드 Post 업데이트->에셋->렌더->실제 렌더링)
	 */
	void Logic();

	// 물체들의 정보를 갱신하는 함수
	bool Update(float DeltaTime);

	/**
	 * 갱신된 정보를 화면에 출력하는 함수
	 * (월드 매니저->렌더 매니저->월드 매니저 포스트 랜더)
	 */
	void Render();

private:
	void InitCDO();

public:
	template <typename T>
	static void CreateCDO()
	{
		CObject::CreateCDO<T>();
	}

	// 윈도우 레지스터 클래스 등록
	void InitRegisterClass(const TCHAR* WindowName, int IconID, int SmallIconID);
	// 윈도우 창 생성
	bool Create(const TCHAR* WindowName, int Width, int Height);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	template <typename T>
	std::weak_ptr<T> CreateEngineSetting()
	{
		mSetting.reset(new T);

		return std::dynamic_pointer_cast<T>(mSetting);
	}

public:
	static CEngine* GetInst()
	{
		if (!mInst)
			mInst = new CEngine;
		return mInst;
	}

	static void DestroyInst()
	{
		SAFE_DELETE(mInst);
	}

private:
	static CEngine* mInst;

private:
	HINSTANCE	mhInst;
	HWND		mhWnd;
	std::shared_ptr<CEngineSetting>	mSetting;

	static bool	mLoop;
	static bool mDeactivateOnce;
};

