#include "Pch.h"
#include "EditorWorld.h"

#include "World/WorldUIManager.h"
#include "Asset/Font/FontManager.h"

#include "Editor/EditorPlayer.h"
#include "UI/Editor/EditorWidget.h"

CEditorWorld::CEditorWorld()
{
}

CEditorWorld::~CEditorWorld()
{
}

bool CEditorWorld::Init()
{
	CWorld::Init();

	LoadFont();
	LoadAnimation2D();
	LoadSound();
	CreateUI();

	mWorldAssetManager->LoadTexture("dirt", TEXT("Tile/dirt.png"));
	mWorldAssetManager->LoadTexture("snow", TEXT("Tile/snow.png"));
	mWorldAssetManager->LoadTexture("scenery", TEXT("Tile/scenery.png"));
	mWorldAssetManager->LoadTexture("girder", TEXT("Tile/girder.png"));
	mWorldAssetManager->LoadTexture("cement", TEXT("Tile/cement.png"));

	mPlayer = CreateGameObject<CEditorPlayer>("Player");

	return true;
}

void CEditorWorld::LoadFont()
{
	std::shared_ptr<CFontManager> FontMgr = CAssetManager::GetInst()->GetFontManager().lock();
	if (FontMgr == nullptr)
	{
		return;
	}

	FontMgr->LoadFont("EngineLight", FontMgr->GetFontFaceName("EngineDefault"), 500, 20.f, TEXT("ko"));
}

void CEditorWorld::LoadAnimation2D()
{
	mWorldAssetManager->CreateAnimation("A_Empty");

	{
		mWorldAssetManager->LoadTexture("T_PlayerIdle", TEXT("Player/Idle/f10.png"));
		mWorldAssetManager->LoadTexture("T_PlayerFrontHair", TEXT("PlayerHair/bangs00.png"));
		mWorldAssetManager->LoadTexture("T_PlayerBackHair", TEXT("PlayerHair/hair00.png"));
	}
}

void CEditorWorld::LoadSound()
{
}

void CEditorWorld::CreateUI()
{
	std::weak_ptr<CEditorWidget> Widget = mUIManager->CreateWidget<CEditorWidget>("EditorWidget");
}