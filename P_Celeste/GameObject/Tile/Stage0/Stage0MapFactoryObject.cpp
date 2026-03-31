#include "Pch.h"
#include "Stage0MapFactoryObject.h"

#include "Component/MeshComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "World/WorldManager.h"

#include "Device.h"
#include "World/World.h"
#include "World/LoadWorld.h"

#include "Character/Player/Player.h"

#include "GameObject/Tile/Stage0/Stage0StaticTileMapObject.h"

#include "UI/Main/SoliloquyWidget.h"

CStage0MapFactoryObject::CStage0MapFactoryObject()
{
	SetClassType<CStage0MapFactoryObject>();
}

bool CStage0MapFactoryObject::InitMapInfos()
{
	InitMapAssets();

	FResolution	RS = CDevice::GetInst()->GetResolution();

	mParallZeroPos = FVector3(RS.Width * 0.5f, RS.Height * 0.5f, 0.f);

	auto StartMap0Info = std::make_shared<FMapInfo<CStage0StaticTileMap0Object>>(FVector2(0.f, 0.f));
	auto StartMap1Info = std::make_shared<FMapInfo<CStage0StaticTileMap1Object>>(FVector2(0.f, 0.f));
	auto StartMap2Info = std::make_shared<FMapInfo<CStage0StaticTileMap2Object>>(FVector2(0.f, 260.f));
	auto StartMap3Info = std::make_shared<FMapInfo<CStage0StaticTileMap3Object>>(FVector2(0.f, 260.f));
	
	StartMap0Info->ConnectMap(
		EMapMoveDirection::Right, 
		StartMap1Info->ConnectMap(
			EMapMoveDirection::Right,
			StartMap2Info->ConnectMap(
				EMapMoveDirection::Right,
				StartMap3Info
			)
		)
	);

	mMapInfos.push_back(StartMap0Info);
	mMapInfos.push_back(StartMap1Info);
	mMapInfos.push_back(StartMap2Info);
	mMapInfos.push_back(StartMap3Info);
	mCurMapInfo = StartMap0Info;

	return true;
}

void CStage0MapFactoryObject::InitMapAssets()
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	{
		WorldAssetMgr->LoadTexture("dirt", TEXT("Tile/dirt.png"));
		WorldAssetMgr->LoadTexture("snow", TEXT("Tile/snow.png"));
		WorldAssetMgr->LoadTexture("scenery", TEXT("Tile/scenery.png"));
	}

	{
		WorldAssetMgr->LoadTexture("T_Stage0_Background0", TEXT("Background/00/bg0.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_Background1", TEXT("Background/00/bg1.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_Background2", TEXT("Background/00/bg2.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_Background3", TEXT("Background/00/bg3.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_Foreground0", TEXT("Background/00/fg0.png"));
	}

	{
		WorldAssetMgr->LoadTexture("T_Stage0_SignPanel", TEXT("Decal/0-prologue/sign.png"));
		
		WorldAssetMgr->LoadTexture("T_Stage0_Fence0", TEXT("Decal/0-prologue/fence_a_left.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_ShortFence0", TEXT("Decal/0-prologue/fence_a_left_short.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_FenceMiddle0", TEXT("Decal/0-prologue/fence_a_middle.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_FenceLeft_Grass0", TEXT("Decal/0-prologue/fence_b_left.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_FenceLeft_Snow0", TEXT("Decal/0-prologue/fence_c_left.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_FenceRight_Snow0", TEXT("Decal/0-prologue/fence_c_right.png"));
		
		WorldAssetMgr->LoadTexture("T_Stage0_Lamp", TEXT("Decal/0-prologue/lamp.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_LampGrass", TEXT("Decal/0-prologue/lamp_grass.png"));

		WorldAssetMgr->LoadTexture("T_Stage0_Sign_LeftArrow", TEXT("Decal/0-prologue/leftArrowSign.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_Sign_RightArrow", TEXT("Decal/0-prologue/rightArrowSign.png"));

		WorldAssetMgr->LoadTexture("T_Stage0_WoodPlatformLeft_2x1", TEXT("CoreObject/platform/wood_2x1_left.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_WoodPlatformRight_2x1", TEXT("CoreObject/platform/wood_2x1_right.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_WoodPlatformLeft_1x1", TEXT("CoreObject/platform/wood_1x1_left.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_WoodPlatformRight_1x1", TEXT("CoreObject/platform/wood_1x1_right.png"));

		WorldAssetMgr->LoadTexture("T_Stage0_OldladyHouse", TEXT("Decal/0-prologue/house.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_Axe", TEXT("Decal/0-prologue/axe.png"));

		WorldAssetMgr->LoadTexture("T_SoliloquyPanel", TEXT("Background/Intro/purplesunset.png"));

		WorldAssetMgr->LoadTexture("T_InputX", TEXT("Input/x.png"));
		WorldAssetMgr->LoadTexture("T_InputZ", TEXT("Input/z.png"));
		WorldAssetMgr->LoadTexture("T_InputUp", TEXT("Input/Up.png"));

		LoadSerialTextures("T_Stage0_BridgeBlock", TEXT("CoreObject/bridge/bridge%d.png"), 11);
	}

	{
		LoadSerialAnimation2D(6, 10, "BirdCry", TEXT("Bird/Cry/crow%d.png"), FVector2(32.f, 24.f));
		LoadSerialAnimation2D(27, 7, "BirdBackFly", TEXT("Bird/BackFly/crow%d.png"), FVector2(32.f, 24.f));
		LoadSerialAnimation2D(0, 6, "BirdIdle", TEXT("Bird/Idle/crow%d.png"), FVector2(32.f, 24.f));
		LoadSerialAnimation2D(16, 11, "BirdPeck", TEXT("Bird/Peck/crow%d.png"), FVector2(32.f, 24.f));
		LoadSerialAnimation2D(0, 10, "BirdFly", TEXT("Bird/Fly/fly%d.png"), FVector2(32.f, 24.f));

		LoadSerialAnimation2D(0, 12, "OldLadyIdle", TEXT("OldLady/Idle/idle%02d.png"), FVector2(32.f, 32.f));
		LoadSerialAnimation2D(0, 4, "OldLadyLaugh", TEXT("OldLady/Laugh/laugh%02d.png"), FVector2(32.f, 32.f));
	}

	{
		WorldAssetMgr->LoadTexture("T_OldLadyPanel", TEXT("Panel/granny.png"));
		
		LoadSerialTextures("T_OldLadyPortraitCreepAIdle", TEXT("Portrait/granny/creepA%02d.png"), 1);
		LoadSerialTextures("T_OldLadyPortraitCreepA", TEXT("Portrait/granny/creepA%02d.png"), 5, 4);
		LoadSerialTextures("T_OldLadyPortraitCreepBIdle", TEXT("Portrait/granny/creepB%02d.png"), 1);
		LoadSerialTextures("T_OldLadyPortraitCreepB", TEXT("Portrait/granny/creepB%02d.png"), 5, 4);
		LoadSerialTextures("T_OldLadyPortraitLaugh", TEXT("Portrait/granny/laugh%02d.png"), 4);
		LoadSerialTextures("T_OldLadyPortraitMock", TEXT("Portrait/granny/mock%02d.png"), 6);
		LoadSerialTextures("T_OldLadyPortraitNormal", TEXT("Portrait/granny/normal%02d.png"), 15);
	}

	{
		WorldAssetMgr->LoadSound("S_Stage0MainBGM", "BGM", true, "BGM/mus_lvl0_intro_loop.wav");
		WorldAssetMgr->LoadSound("S_Stage0BrdigeBGM", "BGM", true, "BGM/mus_lvl0_bridge_oneshot.wav");
	}

	{
		WorldAssetMgr->LoadSound("S_Stage0Intro", "PopUpSFX", false, "SFX/intro_vignette.wav");

		WorldAssetMgr->LoadSound("S_Stage0BridgeRumble", "SFX", true, "SFX/bridge_rumble_loop.wav");

		WorldAssetMgr->LoadSound3D("S_Stage0FallingBlockImpact", "SFX", false, "SFX/Mover/fallingblock_prologue_impact.wav");
		WorldAssetMgr->LoadSound3D("S_Stage0FallingBlockRelease", "SFX", false, "SFX/Mover/fallingblock_prologue_release.wav");
		WorldAssetMgr->LoadSound3D("S_Stage0FallingBlockShake", "SFX", false, "SFX/Mover/fallingblock_prologue_shake.wav");
	}

	{
		WorldAssetMgr->LoadSound3D("S_BirdCry", "SFX", false, "SFX/Bird/squawk.wav");

		LoadSerialSound3Ds("S_BirdPeckA", "SFX", false, "SFX/Bird/peck_a_%02d.wav", 4, 1);
		LoadSerialSound3Ds("S_BirdPeckB", "SFX", false, "SFX/Bird/peck_b_%02d.wav", 4, 1);
		LoadSerialSound3Ds("S_BirdWingFlap", "SFX", false, "SFX/Bird/wingflap_%02d.wav", 6, 1);
		LoadSerialSounds("S_BirdFlyUproll", "SFX", false, "SFX/Bird/flyuproll_%02d.wav", 3, 1);
	}

	{
		WorldAssetMgr->LoadSound3D("S_OldLadyLaugh", "SFX", true, "SFX/OldLady/laugh_firstphrase.wav");

		LoadSerialSounds("S_OldLadyPortraitPerNormal", "SFX", false, "SFX/Portrait/OldLady/normal_per_%02d.wav", 10, 1);
		LoadSerialSounds("S_OldLadyPortraitNormal0_", "SFX", false, "SFX/Portrait/OldLady/normal_mid_A_%02d.wav", 10, 1);
		LoadSerialSounds("S_OldLadyPortraitNormal1_", "SFX", false, "SFX/Portrait/OldLady/normal_mid_B_%02d.wav", 10, 1);
		LoadSerialSounds("S_OldLadyPortraitNormal2_", "SFX", false, "SFX/Portrait/OldLady/normal_mid_C_%02d.wav", 10, 1);

		LoadSerialSounds("S_OldLadyPortraitPerMonk", "SFX", false, "SFX/Portrait/OldLady/mock_end_%02d.wav", 10, 1);
		LoadSerialSounds("S_OldLadyPortraitMonk0_", "SFX", false, "SFX/Portrait/OldLady/mock_mid_A_%02d.wav", 10, 1);
		LoadSerialSounds("S_OldLadyPortraitMonk1_", "SFX", false, "SFX/Portrait/OldLady/mock_mid_B_%02d.wav", 10, 1);
		LoadSerialSounds("S_OldLadyPortraitMonk2_", "SFX", false, "SFX/Portrait/OldLady/mock_mid_C_%02d.wav", 10, 1);

		LoadSerialSounds("S_OldLadyPortraitLaugh0_", "SFX", false, "SFX/Portrait/OldLady/laugh_mid_A_%02d.wav", 10, 1);
		LoadSerialSounds("S_OldLadyPortraitLaugh1_", "SFX", false, "SFX/Portrait/OldLady/laugh_mid_B_%02d.wav", 10, 1);
		LoadSerialSounds("S_OldLadyPortraitLaugh2_", "SFX", false, "SFX/Portrait/OldLady/laugh_mid_C_%02d.wav", 10, 1);
	}
}

void CStage0MapFactoryObject::StartOpening()
{
	std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(mTargetObject.lock());
	Player->GetMovementComp()->SetApplyMove(false);

	std::shared_ptr<CSoliloquyWidget> Widget = mSoliloquyWidget.lock();
	Widget->mOnEndSoliloquy = [TargetObj = mTargetObject, WorldWeakPtr = mWorld]() {
		std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(TargetObj.lock());
		Player->PlayRespawnCutscene();
		Player->GetMovementComp()->SetApplyMove(true);

		std::shared_ptr<CWorld> World = WorldWeakPtr.lock();
		std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();
		CAssetManager::GetInst()->GetSoundManager().lock()->SetGroupVolume("World", 100);

		WorldAssetMgr->SoundPlay("S_Stage0MainBGM", true, 1.f);
		if (WorldAssetMgr->IsSoundPlaying("S_Stage0Intro") == true)
		{
			WorldAssetMgr->SoundStop("S_Stage0Intro", true, 1.f);
		}
		};
	Widget->StartSoliloquy({
		TEXT("여기다, 메들린."),
		TEXT("편하게 숨 쉬자."),
		TEXT("왜 긴장하는거야?")
		});

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();
	WorldAssetMgr->SoundPlay("S_Stage0Intro");

	CAssetManager::GetInst()->GetSoundManager().lock()->SetGroupVolume("World", 0);
};

void CStage0MapFactoryObject::StartEnding()
{
	std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(mTargetObject.lock());
	Player->GetMovementComp()->SetApplyMove(false);

	Player->PlayDeadCutscene([SoliloquyWidget = mSoliloquyWidget]() {
		std::shared_ptr<CSoliloquyWidget> Widget = SoliloquyWidget.lock();
		Widget->mOnEndSoliloquy = []() {
			CAssetManager::GetInst()->GetSoundManager().lock()->SetGroupVolume("World", 100);

			auto World = CWorldManager::GetInst()->CreateWorld<CLoadWorld>(true).lock();
			World->Load(EWorldType::MainStage1);
			};
		Widget->StartSoliloquy({
			TEXT("난 할 수 있어."),
			});
		});

	CAssetManager::GetInst()->GetSoundManager().lock()->SetGroupVolume("World", 0);
}

bool CStage0MapFactoryObject::InitBackgrounds()
{
	FResolution	RS = CDevice::GetInst()->GetResolution();
	FVector2 BackgroundPos = { 0.f, 0.f };
	FVector2 BackgroundScale = { RS.Width * 1.f, RS.Height * 1.f };

	InitSimpleBackground(0, 0, BackgroundPos, BackgroundScale, "Background0", "T_Stage0_Background0", FVector2(1.f, 1.f));
	InitSimpleBackgrounds(0, 0, 6, 1, BackgroundPos, BackgroundScale, "Background1", "T_Stage0_Background1", FVector2(0.95f, 0.95f));
	InitSimpleBackgrounds(0, 0, 6, 1, BackgroundPos, BackgroundScale, "Background2", "T_Stage0_Background2", FVector2(0.6f, 0.6f));
	InitSimpleBackgrounds(0, 0, 6, 1, BackgroundPos, BackgroundScale, "Background3", "T_Stage0_Background3", FVector2(0.3f, 0.3f));
	InitSimpleBackgrounds(0, 0, 6, 1, BackgroundPos, BackgroundScale, "Foreground0", "T_Stage0_Foreground0", FVector2(0.1f, 0.65f));

	return true;
}

bool CStage0MapFactoryObject::InitDynamicObjects()
{
	return true;
}

bool CStage0MapFactoryObject::InitUI()
{
	std::shared_ptr<CWorldUIManager> WorldUIMgr = mWorld.lock()->GetUIManager().lock();
	if (WorldUIMgr == nullptr)
	{
		return false;
	}

	mSoliloquyWidget = WorldUIMgr->CreateWidget<CSoliloquyWidget>("SoliloquyWidget", 3);

	return true;
}


