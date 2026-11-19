#include "Pch.h"
#include "DummyMapFactoryObject.h"

#include <cmath>

#include "Device.h"
#include "World/World.h"
#include "World/WorldAssetManager.h"

#include "GameObject/Tile/Dummy/DummyStaticTileMapObject.h"

CDummyMapFactoryObject::CDummyMapFactoryObject()
{
	SetClassType<CDummyMapFactoryObject>();
}

bool CDummyMapFactoryObject::InitMapInfos()
{
	InitMapAssets();

	const FResolution RS = CDevice::GetInst()->GetResolution();
	mParallZeroPos = FVector3(RS.Width * 0.5f, RS.Height * 0.5f, 0.f);

	const int32 ZoneCount = (std::max)(1, mDummyZoneCount);
	const int32 GridCols = (std::max)(1, static_cast<int32>(std::ceil(std::sqrt(static_cast<double>(ZoneCount)))));
	const int32 GridRows = (ZoneCount + GridCols - 1) / GridCols;

	mMapInfos.reserve(ZoneCount);
	for (int32 i = 0; i < ZoneCount; ++i)
	{
		mMapInfos.push_back(std::make_shared<FMapInfo<CDummyStaticTileMapObject>>(FVector2(0.f, 0.f)));
	}

	auto GetCellIndex = [ZoneCount, GridCols](int32 Row, int32 Col) -> int32
	{
		if (Row < 0 || Col < 0 || Col >= GridCols)
		{
			return -1;
		}
		const int32 Idx = Row * GridCols + Col;
		return (Idx < ZoneCount) ? Idx : -1;
	};

	// 2D 그리드로 8방향 이웃을 모두 양방향 연결하여 9구역 스트리밍 검증 가능
	for (int32 Row = 0; Row < GridRows; ++Row)
	{
		for (int32 Col = 0; Col < GridCols; ++Col)
		{
			const int32 CurIdx = GetCellIndex(Row, Col);
			if (CurIdx < 0)
			{
				continue;
			}

			const std::pair<EMapMoveDirection::Type, std::pair<int32, int32>> ForwardDirs[] =
			{
				{ EMapMoveDirection::Right,		{ 0, 1 } },
				{ EMapMoveDirection::Up,		{ 1, 0 } },
				{ EMapMoveDirection::UpRight,	{ 1, 1 } },
				{ EMapMoveDirection::UpLeft,	{ 1, -1 } },
			};

			for (const auto& Entry : ForwardDirs)
			{
				const int32 NeighborIdx = GetCellIndex(Row + Entry.second.first, Col + Entry.second.second);
				if (NeighborIdx >= 0)
				{
					mMapInfos[CurIdx]->ConnectMap(Entry.first, mMapInfos[NeighborIdx]);
				}
			}
		}
	}

	// 중앙 구역에서 시작해 초기 프레임부터 최대 9구역이 활성화되도록 설정
	const int32 StartRow = GridRows / 2;
	const int32 StartCol = GridCols / 2;
	int32 StartIdx = GetCellIndex(StartRow, StartCol);
	if (StartIdx < 0)
	{
		StartIdx = 0;
	}
	mCurMapInfo = mMapInfos[StartIdx];

	return true;
}

bool CDummyMapFactoryObject::InitBackgrounds()
{
	const FResolution RS = CDevice::GetInst()->GetResolution();
	const FVector2 BackgroundPos = { 0.f, 0.f };
	const FVector2 BackgroundScale = { RS.Width * 1.f, RS.Height * 1.f };

	InitSimpleBackground(0, 0, BackgroundPos, BackgroundScale, "Background0", "T_Stage0_Background0", FVector2(1.f, 1.f));
	InitSimpleBackgrounds(0, 0, 6, 1, BackgroundPos, BackgroundScale, "Background1", "T_Stage0_Background1", FVector2(0.95f, 0.95f));
	InitSimpleBackgrounds(0, 0, 6, 1, BackgroundPos, BackgroundScale, "Background2", "T_Stage0_Background2", FVector2(0.6f, 0.6f));
	InitSimpleBackgrounds(0, 0, 6, 1, BackgroundPos, BackgroundScale, "Background3", "T_Stage0_Background3", FVector2(0.3f, 0.3f));

	return true;
}

bool CDummyMapFactoryObject::InitDynamicObjects()
{
	return true;
}

bool CDummyMapFactoryObject::InitUI()
{
	return true;
}

void CDummyMapFactoryObject::InitMapAssets()
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	WorldAssetMgr->LoadTexture("dirt", TEXT("Tile/dirt.png"));
	WorldAssetMgr->LoadTexture("snow", TEXT("Tile/snow.png"));
	WorldAssetMgr->LoadTexture("scenery", TEXT("Tile/scenery.png"));

	WorldAssetMgr->LoadTexture("T_Stage0_Background0", TEXT("Background/00/bg0.png"));
	WorldAssetMgr->LoadTexture("T_Stage0_Background1", TEXT("Background/00/bg1.png"));
	WorldAssetMgr->LoadTexture("T_Stage0_Background2", TEXT("Background/00/bg2.png"));
	WorldAssetMgr->LoadTexture("T_Stage0_Background3", TEXT("Background/00/bg3.png"));

	WorldAssetMgr->LoadTexture("T_Stage0_Fence0", TEXT("Decal/0-prologue/fence_a_left.png"));
	WorldAssetMgr->LoadTexture("T_Stage0_FenceMiddle0", TEXT("Decal/0-prologue/fence_a_middle.png"));
	WorldAssetMgr->LoadTexture("T_Stage0_Lamp", TEXT("Decal/0-prologue/lamp.png"));
	WorldAssetMgr->LoadTexture("T_Stage0_LampGrass", TEXT("Decal/0-prologue/lamp_grass.png"));
	WorldAssetMgr->LoadTexture("T_Stage0_Sign_RightArrow", TEXT("Decal/0-prologue/rightArrowSign.png"));
	WorldAssetMgr->LoadTexture("T_Stage0_WoodPlatformLeft_2x1", TEXT("CoreObject/platform/wood_2x1_left.png"));
	WorldAssetMgr->LoadTexture("T_Stage0_WoodPlatformRight_2x1", TEXT("CoreObject/platform/wood_2x1_right.png"));

	LoadSerialTextures("T_Stage0_BridgeBlock", TEXT("CoreObject/bridge/bridge%d.png"), 11);
}
