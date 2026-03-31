#include "Pch.h"
#include "WorldLoadThread.h"

#include "World/WorldManager.h"

#include "World/TitleWorld.h"
#include "World/TileMainWorld.h"
#include "World/EditorWorld.h"

#include "GameObject/Tile/Stage0/Stage0MapFactoryObject.h"
#include "GameObject/Tile/Stage1/Stage1MapFactoryObject.h"

#include "Character/Player/Player.h"
#include "Character/Player/TutorialPlayer.h"

CWorldLoadThread::CWorldLoadThread()
{
}

CWorldLoadThread::~CWorldLoadThread()
{
}

void CWorldLoadThread::Exit()
{
	CThreadBase::Exit();
}

void CWorldLoadThread::Run()
{
	switch (mWorldType)
	{
	case EWorldType::Title:
		CWorldManager::GetInst()->CreateAsyncWorld<CTitleWorld>();
		break;
	case EWorldType::MainStage0:
		CWorldManager::GetInst()->CreateAsyncWorld<CTileMainWorld<CStage0MapFactoryObject, CTutorialPlayer>>();
		break;
	case EWorldType::MainStage1:
		CWorldManager::GetInst()->CreateAsyncWorld<CTileMainWorld<CStage1MapFactoryObject>>();
		break;
#ifdef _DEBUG
	case EWorldType::Editor:
		CWorldManager::GetInst()->CreateAsyncWorld<CEditorWorld>();
		break;
#endif // _DEBUG
	}
}
