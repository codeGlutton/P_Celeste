#include "Pch.h"
#include "LoadWorld.h"

#include "ThreadManager.h"
#include "World/WorldManager.h"
#include "Render/RenderManager.h"

#include "Thread/WorldLoadThread.h"

#include "UI/Load/LoadWidget.h"

CLoadWorld::CLoadWorld()
{
}

CLoadWorld::~CLoadWorld()
{
	CThreadManager::GetInst()->DeleteThread("WorldLoadThread");
}

bool CLoadWorld::Init()
{
	CWorld::Init();

	LoadAnimation2D();
	LoadSound();
	CreateUI();

	return true;
}

void CLoadWorld::Update(float DeltaTime)
{
	CWorld::Update(DeltaTime);

	if (mThread->GetComplete())
	{
		CWorldManager::GetInst()->CompleteAsyncWorld();
	}
}

void CLoadWorld::Load(EWorldType WorldType)
{
	mLoadType = WorldType;

	mThread = CThreadManager::GetInst()->Create<CWorldLoadThread>("WorldLoadThread", true);
	mThread->SetWorldType(WorldType);

	mThread->Resume();
}

void CLoadWorld::LoadAnimation2D()
{
}

void CLoadWorld::LoadSound()
{
}

void CLoadWorld::CreateUI()
{
	std::weak_ptr<CLoadWidget> Widget = mUIManager->CreateWidget<CLoadWidget>("LoadWidget");
}