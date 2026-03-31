#include "Pch.h"
#include "TitleWorld.h"

#include "World/WorldUIManager.h"
#include "Asset/Font/FontManager.h"

#include "UI/Title/TitleWidget.h"

CTitleWorld::CTitleWorld()
{
}

CTitleWorld::~CTitleWorld()
{
}

bool CTitleWorld::Init()
{
	CWorld::Init();

	LoadFont();
	LoadAnimation2D();
	LoadSound();
	CreateUI();

	return true;
}

void CTitleWorld::ClearWorld()
{
	mWorldAssetManager->SoundStop("S_MainBGM", true, 1.f);
	CWorld::ClearWorld();
}

void CTitleWorld::LoadFont()
{
	std::shared_ptr<CFontManager> FontMgr = CAssetManager::GetInst()->GetFontManager().lock();
	if (FontMgr == nullptr)
	{
		return;
	}

	FontMgr->LoadFont("EngineLight", FontMgr->GetFontFaceName("EngineDefault"), 500, 20.f, TEXT("ko"));
}

void CTitleWorld::LoadAnimation2D()
{
	mWorldAssetManager->LoadTexture("T_TestBackground", TEXT("TitleUI/test.png"));

	mWorldAssetManager->LoadTexture("T_MainClimbImage", TEXT("TitleUI/start.png"));
	mWorldAssetManager->LoadTexture("T_MainEditorImage", TEXT("TitleUI/pico8.png"));
	mWorldAssetManager->LoadTexture("T_MainExitImage", TEXT("TitleUI/exit.png"));

	mWorldAssetManager->LoadTexture("T_ScreenChange", TEXT("TitleUI/titleScreenChange.png"));
}

void CTitleWorld::LoadSound()
{
	mWorldAssetManager->LoadSound("S_MainBGM", "BGM", true, "BGM/titleLoop.wav");
	mWorldAssetManager->LoadSound("S_MainClimbSelect", "UI", false, "UI/mainClimbSelect.wav");
	mWorldAssetManager->LoadSound("S_MainButtonSelect", "UI", false, "UI/mainButtonSelect.wav");
	mWorldAssetManager->LoadSound("S_MainRollUp", "UI", false, "UI/mainRollUp.wav");
	mWorldAssetManager->LoadSound("S_MainRollDown", "UI", false, "UI/mainRollDown.wav");

	mWorldAssetManager->SoundPlay("S_MainBGM");
}

void CTitleWorld::CreateUI()
{
	mTitleWidget = mUIManager->CreateWidget<CTitleWidget>("TitleWidget");
}