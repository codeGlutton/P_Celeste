#include "Pch.h"
#include "MainWorld.h"

#include "Asset/AssetManager.h"
#include "Asset/Animation2D/Animation2DManager.h"
#include "Asset/Font/FontManager.h"
#include "World/WorldUIManager.h"
#include "Render/RenderManager.h"

#include "Character/Player/Player.h"
#include "GameObject/Tile/Stage0/Stage0StaticTileMapObject.h"

#include "Animation/HairMetaData.h"
#include "TitleWorld.h"

CMainWorld::CMainWorld()
{
}

CMainWorld::~CMainWorld()
{
}

bool CMainWorld::Init()
{
	CWorld::Init();
	LoadAnimation2D();
	LoadFont();
	LoadSound();
	CreateUI();

	return true;
}

void CMainWorld::LoadAnimation2D()
{
	{
		LoadPlayerAnimation2D(9, 10, "PlayerIdle", TEXT("Player/Idle/f%d.png"), TEXT("Player/Idle.meta"));
		LoadPlayerAnimation2D(0, 12, "PlayerWalk", TEXT("Player/RunSlow/f%d.png"), TEXT("Player/Walk.meta"));

		LoadPlayerAnimation2D(0, 8, "PlayerFlip", TEXT("Player/Flip/f%d.png"), TEXT("Player/Flip.meta"));

		LoadPlayerAnimation2D(0, 12, "PlayerWait0", TEXT("Player/IdleA/f%d.png"), TEXT("Player/Wait0.meta"));
		LoadPlayerAnimation2D(0, 24, "PlayerWait1", TEXT("Player/IdleB/f%d.png"), TEXT("Player/Wait1.meta"));
		LoadPlayerAnimation2D(0, 12, "PlayerWait2", TEXT("Player/IdleC/f%d.png"), TEXT("Player/Wait2.meta"));

		LoadPlayerAnimation2D(0, 14, "PlayerEdge", TEXT("Player/Edge/f%d.png"), TEXT("Player/Edge.meta"));
		LoadPlayerAnimation2D(0, 14, "PlayerEdgeBack", TEXT("Player/EdgeBack/f%d.png"), TEXT("Player/EdgeBack.meta"));

		LoadPlayerAnimation2D(0, 2, "PlayerWallJump", TEXT("Player/JumpFast/f%d.png"), TEXT("Player/WallJump.meta"));
		LoadPlayerAnimation2D(0, 2, "PlayerJump", TEXT("Player/JumpSlow/f%d.png"), TEXT("Player/Jump.meta"));
		LoadPlayerAnimation2D(0, 2, "PlayerFall", TEXT("Player/FallFast/f%d.png"), TEXT("Player/Fall.meta"));
		LoadPlayerAnimation2D(0, 7, "PlayerLand", TEXT("Player/Land/f%d.png"), TEXT("Player/Land.meta"));

		LoadPlayerAnimation2D(0, 4, "PlayerDash", TEXT("Player/Dash/f%d.png"), TEXT("Player/Dash.meta"));

		LoadPlayerAnimation2D(0, 1, "PlayerDangle", TEXT("Player/ClimbUp/f%d.png"), TEXT("Player/Dangle.meta"));
		LoadPlayerAnimation2D(0, 10, "PlayerDangleEdge", TEXT("Player/Dangling/f%d.png"), TEXT("Player/DangleEdge.meta"));

		LoadPlayerAnimation2D(0, 1, "PlayerDangleBack", TEXT("Player/ClimbLookBack/f%d.png"), TEXT("Player/DangleBack.meta"));
		LoadPlayerAnimation2D(0, 3, "PlayerDangleBackStart", TEXT("Player/ClimbLookBackStart/f%d.png"), TEXT("Player/DangleBackStart.meta"));

		LoadPlayerAnimation2D(0, 6, "PlayerClimb", TEXT("Player/ClimbUp/f%d.png"), TEXT("Player/Climb.meta"));
		
		LoadPlayerAnimation2D(0, 6, "PlayerLookUp", TEXT("Player/LookUp/f%d.png"), TEXT("Player/LookUp.meta"));
		LoadPlayerAnimation2D(0, 2, "PlayerLookDown", TEXT("Player/Tired/f%d.png"), TEXT("Player/LookDown.meta"));

		LoadPlayerAnimation2D(0, 16, "PlayerPush", TEXT("Player/Push/f%d.png"), TEXT("Player/Push.meta"));

		LoadPlayerAnimation2D(0, 13, "PlayerDead", TEXT("Player/DeadUp/f%d.png"));
	}
	
	{
		LoadPlayerAnimation2D(0, 1, "BreathIdle", TEXT("PlayerSweat/Idle/f%d.png"));
		LoadPlayerAnimation2D(0, 6, "BreathDangle", TEXT("PlayerSweat/Still/f%d.png"));
		LoadPlayerAnimation2D(0, 6, "BreathWarn", TEXT("PlayerSweat/Danger/f%d.png"));
	}

	{
		mWorldAssetManager->LoadTexture("T_PlayerFrontHair", TEXT("PlayerHair/bangs00.png"));
		mWorldAssetManager->LoadTexture("T_PlayerBackHair", TEXT("PlayerHair/hair00.png"));
	}

	{
		mWorldAssetManager->LoadTexture("T_PlayerPanel", TEXT("Panel/madeline.png"));
		mWorldAssetManager->LoadTexture("T_DialogPoint", TEXT("Util/dialog.png"));

		LoadSerialTextures("T_PlayerPortraitAngry", TEXT("Portrait/madeline/angry%02d.png"), 7);
		LoadSerialTextures("T_PlayerPortraitDeadpan", TEXT("Portrait/madeline/deadpan%02d.png"), 9);
		LoadSerialTextures("T_PlayerPortraitDetermined", TEXT("Portrait/madeline/determined%02d.png"), 11);
		LoadSerialTextures("T_PlayerPortraitDistracted", TEXT("Portrait/madeline/distracted%02d.png"), 10);
		LoadSerialTextures("T_PlayerPortraitNormal", TEXT("Portrait/madeline/normal%02d.png"), 7);
		LoadSerialTextures("T_PlayerPortraitPanic", TEXT("Portrait/madeline/panic%02d.png"), 5);
		LoadSerialTextures("T_PlayerPortraitPeaceful", TEXT("Portrait/madeline/peaceful%02d.png"), 4);
		LoadSerialTextures("T_PlayerPortraitSad", TEXT("Portrait/madeline/sad%02d.png"), 7);
		LoadSerialTextures("T_PlayerPortraitSadder", TEXT("Portrait/madeline/sadder%02d.png"), 6);
		LoadSerialTextures("T_PlayerPortraitSurprised", TEXT("Portrait/madeline/surprised%02d.png"), 7);
		LoadSerialTextures("T_PlayerPortraitUpset", TEXT("Portrait/madeline/upset%02d.png"), 7);
	}

	{
		mWorldAssetManager->LoadTexture("T_ScreenChange", TEXT("TitleUI/titleScreenChange.png"));
	}
}

void CMainWorld::LoadFont()
{
	std::shared_ptr<CFontManager> FontMgr = CAssetManager::GetInst()->GetFontManager().lock();
	if (FontMgr == nullptr)
	{
		return;
	}

	FontMgr->LoadFont("EngineLight", FontMgr->GetFontFaceName("EngineDefault"), 500, 20.f, TEXT("ko"));
}

void CMainWorld::LoadSound()
{
	{
		mWorldAssetManager->LoadSound("S_PlayerDash", "SFX", false, "SFX/Player/dash_red.wav");
		mWorldAssetManager->LoadSound("S_PlayerJump", "SFX", false, "SFX/Player/jump.wav");

		LoadSerialSounds("S_PlayerWalk", "SFX", false, "SFX/Player/foot_00_dirt_%02d.wav", 7, 1);
		LoadSerialSounds("S_PlayerLand", "SFX", false, "SFX/Player/land_00_dirt_%02d.wav", 5, 1);
		LoadSerialSounds("S_PlayerClimb", "SFX", false, "SFX/Player/climb_ledge_%02d.wav", 5, 1);
		LoadSerialSounds("S_PlayerGrab", "SFX", false, "SFX/Player/grab_00_dirt_%02d.wav", 5, 1);

		mWorldAssetManager->LoadSound("S_PlayerSlide", "SFX", true, "SFX/Player/wallslide_general.wav");

		LoadSerialSounds("S_PlayerDuck", "SFX", false, "SFX/Player/duck_%02d.wav", 3, 1);

		mWorldAssetManager->LoadSound("S_PlayerDeath", "SFX", false, "SFX/Player/death.wav");
	}

	{
		LoadSerialSounds("S_PlayerPortraitPerNormal", "SFX", false, "SFX/Portrait/Player/normal_per_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitNormal0_", "SFX", false, "SFX/Portrait/Player/normal_mid_A_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitNormal1_", "SFX", false, "SFX/Portrait/Player/normal_mid_B_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitNormal2_", "SFX", false, "SFX/Portrait/Player/normal_mid_C_%02d.wav", 10, 1);

		LoadSerialSounds("S_PlayerPortraitPerAngry", "SFX", false, "SFX/Portrait/Player/angry_per_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitAngry0_", "SFX", false, "SFX/Portrait/Player/angry_mid_A_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitAngry1_", "SFX", false, "SFX/Portrait/Player/angry_mid_B_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitAngry2_", "SFX", false, "SFX/Portrait/Player/angry_mid_C_%02d.wav", 10, 1);

		LoadSerialSounds("S_PlayerPortraitPerUpset", "SFX", false, "SFX/Portrait/Player/upset_per_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitUpset0_", "SFX", false, "SFX/Portrait/Player/upset_mid_A_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitUpset1_", "SFX", false, "SFX/Portrait/Player/upset_mid_B_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitUpset2_", "SFX", false, "SFX/Portrait/Player/upset_mid_C_%02d.wav", 10, 1);

		LoadSerialSounds("S_PlayerPortraitPerSad", "SFX", false, "SFX/Portrait/Player/sad_per_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitSad0_", "SFX", false, "SFX/Portrait/Player/sad_mid_A_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitSad1_", "SFX", false, "SFX/Portrait/Player/sad_mid_B_%02d.wav", 10, 1);
		LoadSerialSounds("S_PlayerPortraitSad2_", "SFX", false, "SFX/Portrait/Player/sad_mid_C_%02d.wav", 10, 1);
	}
}

void CMainWorld::CreateUI()
{
}

void CMainWorld::LoadPlayerAnimation2D(const int32 FileOffset, const int32 FileCount, const std::string& AnimName, const TCHAR* AnimFileNameFormat)
{
	std::string AnimAssetName = "A_" + AnimName;
	std::string TextureAssetName = "T_" + AnimName;

	mWorldAssetManager->CreateAnimation(AnimAssetName);
	mWorldAssetManager->SetAnimation2DTextureType(AnimAssetName, EAnimation2DTextureType::Frame);

	std::vector<const TCHAR*> TextureFileNames;
	for (int i = 0; i < FileCount; ++i)
	{
		TCHAR* FileName = new TCHAR[MAX_PATH];
		memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
		wsprintf(FileName, AnimFileNameFormat, i + FileOffset);
		TextureFileNames.push_back(FileName);
	}

	mWorldAssetManager->SetTexture(AnimAssetName, TextureAssetName, TextureFileNames);

	for (int i = 0; i < FileCount; ++i)
	{
		delete[] TextureFileNames[i];
	}
	TextureFileNames.clear();

	mWorldAssetManager->AddFrame(AnimAssetName, FileCount, 0.f, 0.f, 32.f, 32.f);
}

void CMainWorld::LoadPlayerAnimation2D(const int32 FileOffset, const int32 FileCount, const std::string& AnimName, const TCHAR* AnimFileNameFormat, const TCHAR* MetaFileName)
{
	std::string AnimAssetName = "A_" + AnimName;
	mWorldAssetManager->LoadAnimMetaData<CHairMetaData>(AnimAssetName, MetaFileName);
	LoadPlayerAnimation2D(FileOffset, FileCount, AnimName, AnimFileNameFormat);
}

void CMainWorld::LoadSerialTextures(const std::string& TextureName, const std::wstring& FilePathFormat, int32 Count, int32 StartOffset)
{
	for (int i = 0; i < Count; ++i)
	{
		TCHAR* FileName = new TCHAR[MAX_PATH];
		memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
		wsprintf(FileName, FilePathFormat.c_str(), i + StartOffset);

		mWorldAssetManager->LoadTexture(TextureName + std::to_string(i), FileName);

		delete[] FileName;
	}
}

void CMainWorld::LoadSerialSounds(const std::string& TextureName, const std::string& GroupName, bool IsLoop, const std::string& FilePathFormat, int32 Count, int32 StartOffset)
{
	for (int i = 0; i < Count; ++i)
	{
		char* FileName = new char[MAX_PATH];
		memset(FileName, 0, MAX_PATH);
		sprintf_s(FileName, MAX_PATH, FilePathFormat.c_str(), i + StartOffset);

		mWorldAssetManager->LoadSound(TextureName + std::to_string(i), GroupName, IsLoop, FileName);

		delete[] FileName;
	}
}

void CMainWorld::LoadSerialSound3Ds(const std::string& TextureName, const std::string& GroupName, bool IsLoop, const std::string& FilePathFormat, int32 Count, int32 StartOffset)
{
	for (int i = 0; i < Count; ++i)
	{
		char* FileName = new char[MAX_PATH];
		memset(FileName, 0, MAX_PATH);
		sprintf_s(FileName, MAX_PATH, FilePathFormat.c_str(), i + StartOffset);

		mWorldAssetManager->LoadSound3D(TextureName + std::to_string(i), GroupName, IsLoop, FileName);

		delete[] FileName;
	}
}
