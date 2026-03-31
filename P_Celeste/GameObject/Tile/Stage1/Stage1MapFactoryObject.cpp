#include "Pch.h"
#include "Stage1MapFactoryObject.h"

#include "Component/MeshComponent.h"

#include "Device.h"
#include "World/World.h"

#include "GameObject/Tile/Stage1/Stage1StaticTileMapObject.h"

CStage1MapFactoryObject::CStage1MapFactoryObject()
{
	SetClassType<CStage1MapFactoryObject>();
}

bool CStage1MapFactoryObject::InitMapInfos()
{
	InitMapAssets();

	FResolution	RS = CDevice::GetInst()->GetResolution();

	mParallZeroPos = FVector3(RS.Width * 0.5f, RS.Height * 0.5f, 0.f);

	auto StartMap0Info = std::make_shared<FMapInfo<CStage1StaticTileMap0Object>>(FVector2(0.f, 0.f));
	auto StartMap1Info = std::make_shared<FMapInfo<CStage1StaticTileMap1Object>>(FVector2(780.f, 0.f));
	auto StartMap2Info = std::make_shared<FMapInfo<CStage1StaticTileMap2Object>>(FVector2(1586.f, 0.f));
	
	StartMap0Info->ConnectMap(
		EMapMoveDirection::Up,
		StartMap1Info->ConnectMap(
			EMapMoveDirection::Up,
			StartMap2Info
		)
	);
	
	mMapInfos.push_back(StartMap0Info);
	mMapInfos.push_back(StartMap1Info);
	mMapInfos.push_back(StartMap2Info);
	mCurMapInfo = StartMap0Info;

	return true;
}

bool CStage1MapFactoryObject::InitBackgrounds()
{
	FResolution	RS = CDevice::GetInst()->GetResolution();
	FVector2 BackgroundPos = { 0.f, 0.f };
	FVector2 BackgroundScale = { RS.Width * 1.f, RS.Height * 1.f };

	InitSimpleBackground(0, 0, BackgroundPos, BackgroundScale, "Background0", "T_Stage1_Background0", FVector2(1.f, 1.f));
	InitSimpleBackgrounds(0, 0, 4, 1, BackgroundPos, BackgroundScale, "Background1", "T_Stage1_Background1", FVector2(0.95f, 0.95f));
	InitSimpleBackgrounds(0, 0, 4, 1, BackgroundPos, BackgroundScale, "Background2", "T_Stage1_Background2", FVector2(0.8f, 0.8f));
	InitSimpleBackgrounds(0, 0, 6, 1, BackgroundPos, BackgroundScale, "Background3", "T_Stage0_Background3", FVector2(0.5f, 0.5f));

	return true;
}

bool CStage1MapFactoryObject::InitDynamicObjects()
{
	return true;
}

bool CStage1MapFactoryObject::InitUI()
{
	return true;
}

void CStage1MapFactoryObject::InitMapAssets()
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	{
		WorldAssetMgr->LoadTexture("dirt", TEXT("Tile/dirt.png"));
		WorldAssetMgr->LoadTexture("snow", TEXT("Tile/snow.png"));
		WorldAssetMgr->LoadTexture("scenery", TEXT("Tile/scenery.png"));
		WorldAssetMgr->LoadTexture("girder", TEXT("Tile/girder.png"));
		WorldAssetMgr->LoadTexture("cement", TEXT("Tile/cement.png"));
	}

	{
		WorldAssetMgr->LoadTexture("T_Stage1_Background0", TEXT("Background/01/bg0.png"));
		WorldAssetMgr->LoadTexture("T_Stage1_Background1", TEXT("Background/01/bg1.png"));
		WorldAssetMgr->LoadTexture("T_Stage1_Background2", TEXT("Background/01/bg2.png"));
		WorldAssetMgr->LoadTexture("T_Stage0_Background3", TEXT("Background/00/bg3.png"));
	}

	{
		LoadSerialAnimation2D(0, 7, "StrawberryIdle", TEXT("Collectable/strawberry/normal%02d.png"), FVector2(58.5f, 52.f));
		LoadSerialAnimation2D(7, 13, "StrawberryGet", TEXT("Collectable/strawberry/normal%02d.png"), FVector2(58.5f, 52.f));

		LoadSerialAnimation2D(0, 7, "GhostberryIdle", TEXT("Collectable/ghostberry/idle%02d.png"), FVector2(58.5f, 52.f));
		LoadSerialAnimation2D(7, 13, "GhostberryGet", TEXT("Collectable/ghostberry/idle%02d.png"), FVector2(58.5f, 52.f));

		LoadSerialAnimation2D(0, 7, "WoodSpring", TEXT("CoreObject/spring/%02d.png"), FVector2(52.f, 52.f));
	}

	{
		WorldAssetMgr->LoadTexture("T_Stage1_SignConstructionPanel", TEXT("Decal/1-forsakencity/sign_under_construction.png"));
		WorldAssetMgr->LoadTexture("T_Stage1_SignForwardPanel", TEXT("Decal/1-forsakencity/sign_foward.png"));
		WorldAssetMgr->LoadTexture("T_Stage1_Ladder", TEXT("Decal/1-forsakencity/ladder.png"));
		WorldAssetMgr->LoadTexture("T_Stage1_PaintBuckets", TEXT("Decal/1-forsakencity/paint_buckets.png"));
		WorldAssetMgr->LoadTexture("T_Stage1_TraficLightOff", TEXT("Decal/1-forsakencity/trafic_lights_ceiling_busted.png"));
		WorldAssetMgr->LoadTexture("T_Stage1_SignPark", TEXT("Decal/1-forsakencity/sign_you_can_park.png"));

		WorldAssetMgr->LoadTexture("T_Stage1_WoodPlatform_4x1", TEXT("CoreObject/platform/wood_4x1.png"));
		WorldAssetMgr->LoadTexture("T_Stage1_WoodPlatform_5x1", TEXT("CoreObject/platform/wood_5x1.png"));
	}

	{
		WorldAssetMgr->LoadSound("S_Stage1MainPianoBGM", "BGM", true, "BGM/mus_lvl1_main_piano_loop.wav");
		WorldAssetMgr->LoadSound("S_Stage1MainPercBGM", "BGM", true, "BGM/mus_lvl1_main_perc_loop.wav");
		WorldAssetMgr->LoadSound("S_Stage1MainSynthsBGM", "BGM", true, "BGM/mus_lvl1_main_synths_loop.wav");
	}

	{
		WorldAssetMgr->LoadSound3D("S_Spring", "SFX", false, "SFX/Mover/spring.wav");

		WorldAssetMgr->LoadSound3D("S_StrawberryTouch", "SFX", false, "SFX/Strawberry/strawberry_touch.wav");
		WorldAssetMgr->LoadSound3D("S_GhostberryTouch", "SFX", false, "SFX/Strawberry/strawberry_blue_touch.wav");
		WorldAssetMgr->LoadSound3D("S_StrawberryGet", "SFX", false, "SFX/Strawberry/strawberry_red_get_1000.wav");
		WorldAssetMgr->LoadSound3D("S_GhostberryGet", "SFX", false, "SFX/Strawberry/strawberry_blu_get_1000.wav");
	}
}

void CStage1MapFactoryObject::StartOpening()
{
	CMapFactoryObject::StartOpening();

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();
	WorldAssetMgr->SoundPlay("S_Stage1MainPianoBGM", true, 1.f);
	WorldAssetMgr->SoundPlay("S_Stage1MainPercBGM", true, 1.f);
	WorldAssetMgr->SoundPlay("S_Stage1MainSynthsBGM", true, 1.f);
}

void CStage1MapFactoryObject::StartEnding()
{
	CMapFactoryObject::StartEnding();
}



