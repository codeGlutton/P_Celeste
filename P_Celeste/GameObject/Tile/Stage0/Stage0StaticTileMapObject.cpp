#include "Pch.h"
#include "Stage0StaticTileMapObject.h"

#include "Component/TileMapRender.h"
#include "Component/TileMapComponent.h"
#include "Component/ColliderBox2D.h"
#include "Component/DamageBoxCollider.h"
#include "Component/WorldTimeChangableBoxCollider.h"
#include "Component/ColliderOneWayBox2D.h"
#include "Component/MeshComponent.h"
#include "Component/TileMapCameraComponent.h"

#include "Character/NPC/Bird.h"
#include "Character/NPC/OldLady.h"
#include "Character/Player/TutorialPlayer.h"

#include "Component/CharacterMovementComponent.h"
#include "Movement/PlayerGameState.h"

#include "Dialogue/DialogueData.h"

#include "World/TileMainWorld.h"

// 0번째
#pragma region CStage0StaticTileMap0Object

CStage0StaticTileMap0Object::CStage0StaticTileMap0Object()
{
	SetClassType<CStage0StaticTileMap0Object>();
}

bool CStage0StaticTileMap0Object::Init()
{
	CStaticTileMapObject::Init();

	mPlayerRespawnOffsets[EMapMoveDirection::Left] = FVector3(52.f, 130.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::Right] = FVector3(1500.f, 110.f, 0.f);

	if (InitMainTileMap() == false)
	{
		return false;
	}
	if (InitCollisions() == false)
	{
		return false;
	}
	if (InitPlatforms() == false)
	{
		return false;
	}
	if (InitEnvMeshes() == false)
	{
		return false;
	}
	if (InitBlockers() == false)
	{
		return false;
	}
	if (InitDynamicObjects() == false)
	{
		return false;
	}

	return true;
}

void CStage0StaticTileMap0Object::LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData)
{
	CStaticTileMapObject::LoadMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage0StaticTileMap0Object::FMapRuntimeData>(RuntimeData);
	auto IceBlock = std::static_pointer_cast<CTriggerDropTileMapObject>(FindSubGameObject("DropIceBlock").lock());
	if (IceBlock != nullptr)
	{
		IceBlock->LoadMapRuntimeData(CurRuntimeData->mIceBlockData);
	}
}

void CStage0StaticTileMap0Object::SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const
{
	CStaticTileMapObject::SaveMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage0StaticTileMap0Object::FMapRuntimeData>(RuntimeData);
	auto IceBlock = std::static_pointer_cast<const CTriggerDropTileMapObject>(FindSubGameObject("DropIceBlock").lock());
	if (IceBlock != nullptr)
	{
		IceBlock->SaveMapRuntimeData(CurRuntimeData->mIceBlockData);
	}
}

bool CStage0StaticTileMap0Object::InitMainTileMap()
{
	{
		FTileMapLayer& MapLayer = AddTileLayer("Ice");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile0");
		TileRenderComp->EnableTileAlphaBlend();
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Ice", TEXT("tutorial_ice.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Dirt");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile1");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Dirt", TEXT("tutorial_rock.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Snow");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile2");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Snow", TEXT("tutorial_snow.tlm"));
	}

	return true;
}

bool CStage0StaticTileMap0Object::InitCollisions()
{
	static const std::vector<std::pair<FVector2, FVector2>> WallTransforms =
	{
		{{442.f, 156.f}, {221.f, 78.f}},
		{{130.f, 78.f}, {403.f, 169.f}},
		{{156.f, 130.f}, {676.f, 65.f}},
		{{52.f, 78.f}, {702.f, 169.f}},
		{{26.f, 156.f}, {741.f, 78.f}},
		{{78.f, 52.f}, {637.f, 390.f}},
		{{130.f, 182.f}, {923.f, 91.f}},
		{{182.f, 26.f}, {923.f, 169.f}},
		{{26.f, 52.f}, {1001.f, 156.f}},
		{{338.f, 104.f}, {871.f, 598.f}},
		{{78.f, 78.f}, {1053.f, 611.f}},
		{{208.f, 52.f}, {910.f, 520.f}},
		{{52.f, 130.f}, {728.f, 585.f}},
		{{182.f, 78.f}, {923.f, 507.f}},
		{{104.f, 78.f}, {1196.f, 91.f}},
		
		{{52.f, 130.f}, {1248.f, 143.f}},
		{{78.f, 130.f}, {1209.f, 65.f}},
		{{208.f, 26.f}, {1456.f, 91.f}},
		{{182.f, 104.f}, {1469.f, 52.f}},
	};

	for (auto& WallTransform : WallTransforms)
	{
		std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Wall").lock();

		WallBoxComp->SetCollisionProfile("Static");
		WallBoxComp->SetBoxSize(WallTransform.first);
		WallBoxComp->SetRelativePos(WallTransform.second);
#ifdef _DEBUG

		WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
		WallBoxComp->SetInheritScale(false);
		WallBoxComp->SetStatic(true);
	}

	return true;
}

bool CStage0StaticTileMap0Object::InitPlatforms()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> PlatformObjectSets =
	{
		{{1118.f, 104.f},	{26.f, 26.f},	"T_Stage0_WoodPlatformRight_1x1"},
	};

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (auto& PlatformObjectSet : PlatformObjectSets)
	{
		std::shared_ptr<CMeshComponent> MeshComponent = CreateComponent<CMeshComponent>("Platform").lock();
		MeshComponent->SetShader("DefaultTexture2D");
		MeshComponent->SetMesh("RectTex");
		MeshComponent->SetRelativePos(std::get<0>(PlatformObjectSet));
		MeshComponent->SetWorldScale(std::get<1>(PlatformObjectSet));
		MeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		MeshComponent->SetBlendState(0, "AlphaBlend");
		MeshComponent->SetRenderLayer("Map");
		MeshComponent->SetPivot(0.f, 0.f);
		MeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture(std::get<2>(PlatformObjectSet)));

		std::shared_ptr<CColliderOneWayBox2D> OneWayBoxComp = CreateComponent<CColliderOneWayBox2D>("PlatformCollider").lock();

		FVector2 mPlatformCollisionPos = {
			std::get<0>(PlatformObjectSet).x + std::get<1>(PlatformObjectSet).x * 0.5f,
			std::get<0>(PlatformObjectSet).y + std::get<1>(PlatformObjectSet).y - 2.5f
		};

		OneWayBoxComp->SetCollisionProfile("Static");
		OneWayBoxComp->SetOneWayAxis(FVector3::Axis[EAxis::Y]);
		OneWayBoxComp->SetRelativePos(mPlatformCollisionPos);
		OneWayBoxComp->SetBoxSize(std::get<1>(PlatformObjectSet).x, 5.f);
#ifdef _DEBUG

		OneWayBoxComp->SetDebugDraw(true);

#endif // DEBUG
		OneWayBoxComp->SetInheritScale(false);
		OneWayBoxComp->SetStatic(true);
	}

	return true;
}

bool CStage0StaticTileMap0Object::InitEnvMeshes()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> EnvObjectSets =
	{
		{{403.f, 208.f},	{117.f, 65.f},	"T_Stage0_SignPanel" },

		{{1430.f, 104.f},	{52.f, 52.f},	"T_Stage0_Fence0"},
		{{1482.f, 104.f},	{52.f, 52.f},	"T_Stage0_FenceMiddle0"},
		{{1534.f, 104.f},	{52.f, 52.f},	"T_Stage0_FenceMiddle0"},

		{{1378.f, 104.f},	{52.f, 52.f},	"T_Stage0_Sign_RightArrow"},

		{{286.f, 156.f},	{52.f, 260.f},	"T_Stage0_Lamp"},
		{{1248.f, 208.f},	{52.f, 260.f},	"T_Stage0_LampGrass"},
	};

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (auto& EnvObjectSet : EnvObjectSets)
	{
		std::shared_ptr<CMeshComponent> MeshComponent = CreateComponent<CMeshComponent>("EnvObject").lock();
		MeshComponent->SetShader("DefaultTexture2D");
		MeshComponent->SetMesh("RectTex");
		MeshComponent->SetRelativePos(std::get<0>(EnvObjectSet));
		MeshComponent->SetWorldScale(std::get<1>(EnvObjectSet));
		MeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		MeshComponent->SetBlendState(0, "AlphaBlend");
		MeshComponent->SetRenderLayer("Map");
		MeshComponent->SetPivot(0.5f, 0.f);
		MeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture(std::get<2>(EnvObjectSet)));
	}

	return true;
}

bool CStage0StaticTileMap0Object::InitBlockers()
{
	static const std::vector<std::pair<FVector2, FVector2>> BlockerTransforms =
	{
		{{50.f, 650.f},		{-25.f, 325.f}},
		{{1560.f, 50.f},	{780.f, 675.f}},
	};

	for (auto& WallTransform : BlockerTransforms)
	{
		std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Blocker").lock();

		WallBoxComp->SetCollisionProfile("Blocker");
		WallBoxComp->SetBoxSize(WallTransform.first);
		WallBoxComp->SetRelativePos(WallTransform.second);
#ifdef _DEBUG

		WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
		WallBoxComp->SetInheritScale(false);
		WallBoxComp->SetStatic(true);
	}

	std::shared_ptr<CDamageBoxCollider> FloorBoxComp = CreateComponent<CDamageBoxCollider>("FloorBlocker").lock();
	FloorBoxComp->SetBoxSize(1560.f, 50.f);
	FloorBoxComp->SetRelativePos(780.f, -40.f);

	return true;
}

bool CStage0StaticTileMap0Object::InitDynamicObjects()
{
	{
		FTriggerDropTileObjectInitInfo InitInfo;

		InitInfo.mWorldPos = FVector3(833.f, 366.f, 0.f);
		InitInfo.mCollisionSize = { 182.f, 104.f };

		std::vector<FDynamicTileLayerInitInfo> LayerInitInfo =
		{
			{TEXT("tutorial_block0_snow.tlm"), "snow", "FrontTile2", false},
		};

		InitInfo.mLayerInitInfos = LayerInitInfo;
		InitInfo.mDropGravityScale = 2.f;
		InitInfo.mDropReadyMaxTime = 0.4f;

		InitInfo.mTriggerBoxWorldPos = FVector3(859.f, 182.f, 0.f);
		InitInfo.mTriggerBoxSize = { 124.f, 104.f };

		InitInfo.mShakeSound = "S_Stage0FallingBlockShake";
		InitInfo.mDropSound = "S_Stage0FallingBlockRelease";
		InitInfo.mHitSound = "S_Stage0FallingBlockImpact";

		AddSubGameObject("DropIceBlock", CTriggerDropTileMapObject::MakeQuikly(mWorld.lock(), InitInfo));
	}

	return true;
}

#pragma endregion

// 1번쩨
#pragma region CStage0StaticTileMap1Object

CStage0StaticTileMap1Object::CStage0StaticTileMap1Object()
{
	SetClassType<CStage0StaticTileMap1Object>();
}

bool CStage0StaticTileMap1Object::Init()
{
	CStaticTileMapObject::Init();

	mPlayerRespawnOffsets[EMapMoveDirection::Left] = FVector3(40.f, 105.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::Right] = FVector3(1500.f, 364.f, 0.f);

	if (InitMainTileMap() == false)
	{
		return false;
	}
	if (InitCollisions() == false)
	{
		return false;
	}
	if (InitPlatforms() == false)
	{
		return false;
	}
	if (InitEnvMeshes() == false)
	{
		return false;
	}
	if (InitBlockers() == false)
	{
		return false;
	}
	if (InitNPCs() == false)
	{
		return false;
	}

	return true;
}

void CStage0StaticTileMap1Object::LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData)
{
	CStaticTileMapObject::LoadMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage0StaticTileMap1Object::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData != nullptr)
	{
		if (CurRuntimeData->mShowTutorial == true)
		{
			RemoveTutorial();
			std::shared_ptr<CBird> Bird = mTutorialBird.lock();
			if (Bird != nullptr)
			{
				Bird->SetEnable(false);
			}
		}
		else
		{
			RollBackTutorial();
		}
	}
}

void CStage0StaticTileMap1Object::SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const
{
	CStaticTileMapObject::SaveMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage0StaticTileMap1Object::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData != nullptr)
	{
		CurRuntimeData->mShowTutorial = mTutirialTriggers.empty();
	}
}

bool CStage0StaticTileMap1Object::InitMainTileMap()
{
	{
		FTileMapLayer& MapLayer = AddTileLayer("Ice");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile0");
		TileRenderComp->EnableTileAlphaBlend();
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Ice", TEXT("tutorial_ice_2.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Dirt");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile1");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Dirt", TEXT("tutorial_rock_2.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Snow");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile2");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Snow", TEXT("tutorial_snow_2.tlm"));
	}

	return true;
}

bool CStage0StaticTileMap1Object::InitCollisions()
{
	static const std::vector<std::pair<FVector2, FVector2>> WallTransforms =
	{
		{{182.f, 104.f},	{91.f, 52.f}},
		{{208.f, 26.f},		{104.f, 91.f}},
		{{104.f, 26.f},		{364.f, 117.f}},
		{{156.f, 130.f},	{416.f, 65.f}},
		{{104.f, 234.f},	{468.f, 195.f}},
		{{130.f, 104.f},	{481.f, 260.f}},
		{{104.f, 52.f},		{676.f, 180.f}},
		{{52.f, 208.f},		{676.f, 104.f}},
		{{78.f, 78.f},		{689.f, 168.f}},
		{{52.f, 156.f},		{884.f, 312.f}},
		{{52.f, 234.f},		{910.f, 195.f}},
		{{156.f, 156.f},	{962.f, 156.f}},
		{{130.f, 78.f},		{975.f, 39.f}},
		{{156.f, 26.f},		{1196.f, 91.f}},
		{{78.f, 104.f},		{1209.f, 52.f}},
		{{52.f, 78.f},		{1222.f, 91.f}},
		{{52.f, 130.f},		{1274.f, 117.f}},
		{{52.f, 26.f},		{1300.f, 169.f}},

		{{52.f, 260.f},		{1196.f, 416.f}},
		{{78.f, 156.f},		{1209.f, 468.f}},

		{{52.f, 104.f},		{1430.f, 156.f}},
		{{130.f, 338.f},	{1495.f, 169.f}},
	};

	for (auto& WallTransform : WallTransforms)
	{
		std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Wall").lock();

		WallBoxComp->SetCollisionProfile("Static");
		WallBoxComp->SetBoxSize(WallTransform.first);
		WallBoxComp->SetRelativePos(WallTransform.second);
#ifdef _DEBUG

		WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
		WallBoxComp->SetInheritScale(false);
		WallBoxComp->SetStatic(true);
	}

	return true;
}

bool CStage0StaticTileMap1Object::InitPlatforms()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> PlatformObjectSets =
	{
		{{832.f, 78.f},	{52.f, 26.f},	"T_Stage0_WoodPlatformRight_2x1"},
	};

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (auto& PlatformObjectSet : PlatformObjectSets)
	{
		std::shared_ptr<CMeshComponent> MeshComponent = CreateComponent<CMeshComponent>("Platform").lock();
		MeshComponent->SetShader("DefaultTexture2D");
		MeshComponent->SetMesh("RectTex");
		MeshComponent->SetRelativePos(std::get<0>(PlatformObjectSet));
		MeshComponent->SetWorldScale(std::get<1>(PlatformObjectSet));
		MeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		MeshComponent->SetBlendState(0, "AlphaBlend");
		MeshComponent->SetRenderLayer("Map");
		MeshComponent->SetPivot(0.f, 0.f);
		MeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture(std::get<2>(PlatformObjectSet)));

		std::shared_ptr<CColliderOneWayBox2D> OneWayBoxComp = CreateComponent<CColliderOneWayBox2D>("PlatformCollider").lock();

		FVector2 mPlatformCollisionPos = {
			std::get<0>(PlatformObjectSet).x + std::get<1>(PlatformObjectSet).x * 0.5f,
			std::get<0>(PlatformObjectSet).y + std::get<1>(PlatformObjectSet).y - 2.5f
		};

		OneWayBoxComp->SetCollisionProfile("Static");
		OneWayBoxComp->SetOneWayAxis(FVector3::Axis[EAxis::Y]);
		OneWayBoxComp->SetRelativePos(mPlatformCollisionPos);
		OneWayBoxComp->SetBoxSize(std::get<1>(PlatformObjectSet).x, 5.f);
#ifdef _DEBUG

		OneWayBoxComp->SetDebugDraw(true);

#endif // DEBUG
		OneWayBoxComp->SetInheritScale(false);
		OneWayBoxComp->SetStatic(true);
	}

	return true;
}

bool CStage0StaticTileMap1Object::InitEnvMeshes()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> EnvObjectSets =
	{
		{{130.f, 104.f},		{52.f, 260.f},		"T_Stage0_LampGrass"},

		{{26.f, 104.f},			{52.f, 52.f},		"T_Stage0_FenceMiddle0"},
		{{78.f, 104.f},			{52.f, 52.f},		"T_Stage0_Fence0"},
		{{650.f, 208.f},		{52.f, 52.f},		"T_Stage0_FenceLeft_Snow0"},
		{{702.f, 208.f},		{52.f, 52.f},		"T_Stage0_FenceRight_Snow0"},
		
		{{1274.f, 182.f},		{26.f, 52.f},		"T_Stage0_ShortFence0"},
		{{1300.f, 182.f},		{52.f, 52.f},		"T_Stage0_FenceLeft_Grass0"},
		
		{{1417.f, 208.f},		{26.f, 52.f},		"T_Stage0_ShortFence0"},
		
		{{1456.f, 338.f},		{26.f, 52.f},		"T_Stage0_ShortFence0"},
		{{1482.f, 338.f},		{52.f, 52.f},		"T_Stage0_FenceMiddle0"},
		{{1534.f, 338.f},		{52.f, 52.f},		"T_Stage0_FenceMiddle0"},
	};

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (auto& EnvObjectSet : EnvObjectSets)
	{
		std::shared_ptr<CMeshComponent> MeshComponent = CreateComponent<CMeshComponent>("EnvObject").lock();
		MeshComponent->SetShader("DefaultTexture2D");
		MeshComponent->SetMesh("RectTex");
		MeshComponent->SetRelativePos(std::get<0>(EnvObjectSet));
		MeshComponent->SetWorldScale(std::get<1>(EnvObjectSet));
		MeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		MeshComponent->SetBlendState(0, "AlphaBlend");
		MeshComponent->SetRenderLayer("Map");
		MeshComponent->SetPivot(0.5f, 0.f);
		MeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture(std::get<2>(EnvObjectSet)));
	}

	return true;
}

bool CStage0StaticTileMap1Object::InitBlockers()
{
	static const std::vector<std::pair<FVector2, FVector2>> BlockerTransforms =
	{
		{{1560.f, 50.f},	{780.f, 675.f}},
	};

	for (auto& WallTransform : BlockerTransforms)
	{
		std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Blocker").lock();

		WallBoxComp->SetCollisionProfile("Blocker");
		WallBoxComp->SetBoxSize(WallTransform.first);
		WallBoxComp->SetRelativePos(WallTransform.second);
#ifdef _DEBUG

		WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
		WallBoxComp->SetInheritScale(false);
		WallBoxComp->SetStatic(true);
	}

	std::shared_ptr<CDamageBoxCollider> FloorBoxComp = CreateComponent<CDamageBoxCollider>("FloorBlocker").lock();
	FloorBoxComp->SetBoxSize(1560.f, 50.f);
	FloorBoxComp->SetRelativePos(780.f, -40.f);

	return true;
}

bool CStage0StaticTileMap1Object::InitNPCs()
{
	std::weak_ptr<CBird> BirdWeakPtr = mWorld.lock()->CreateGameObject<CBird>("Bird");
	mTutorialBird = BirdWeakPtr;

	std::shared_ptr<CBird> BirdSharedPtr = BirdWeakPtr.lock();
	BirdSharedPtr->SetWorldPos(GetWorldPos() + FVector3(468.f, 312.f, 0.f));
	BirdSharedPtr->SetSightDir(-FVector3::Axis[EAxis::X]);

	std::shared_ptr<CColliderBox2D> DangerTrigger = CreateComponent<CColliderBox2D>("BirdDangerTrigger").lock();
	{
		mTutirialTriggers.push_back(DangerTrigger);

		DangerTrigger->SetRelativePos(364.f, 130.f);
		DangerTrigger->SetBoxSize(104.f, 13.f);
		DangerTrigger->SetCollisionProfile("EventTrigger");
		DangerTrigger->SetCollisionBeginFunction([BirdWeakPtr](const FVector3& HitPos, CCollider* Collider) {
			std::shared_ptr<CBird> BirdSharedPtr = BirdWeakPtr.lock();
			if (BirdSharedPtr == nullptr)
			{
				return;
			}
			BirdSharedPtr->Cry();
			BirdSharedPtr->StartTalk(TEXT("오르기"), TEXT("길게 누르기"), "T_InputX");
			});

#ifdef _DEBUG

		DangerTrigger->SetDebugDraw(true);

#endif // _DEBUG
	}

	std::shared_ptr<CColliderBox2D> ClimbTrigger = CreateComponent<CColliderBox2D>("BirdClimbTrigger").lock();
	{
		mTutirialTriggers.push_back(ClimbTrigger);

		ClimbTrigger->SetRelativePos(410.f, 260.f);
		ClimbTrigger->SetBoxSize(13.f, 104.f);
		ClimbTrigger->SetCollisionProfile("EventTrigger");
		ClimbTrigger->SetCollisionBeginFunction([BirdWeakPtr](const FVector3& HitPos, CCollider* Collider) {
			std::shared_ptr<CBird> BirdSharedPtr = BirdWeakPtr.lock();
			if (BirdSharedPtr == nullptr)
			{
				return;
			}
			BirdSharedPtr->Cry();
			BirdSharedPtr->StartTalk(TEXT("오르기"), TEXT("유지한 채"), "T_InputUp");
			});

#ifdef _DEBUG

		ClimbTrigger->SetDebugDraw(true);

#endif // _DEBUG
	}

	std::shared_ptr<CColliderBox2D> FlyTrigger = CreateComponent<CColliderBox2D>("BirdFlyTrigger").lock();
	{
		mTutirialTriggers.push_back(FlyTrigger);

		FlyTrigger->SetRelativePos(514.f, 364.f);
		FlyTrigger->SetBoxSize(104.f, 104.f);
		FlyTrigger->SetCollisionProfile("EventTrigger");
		FlyTrigger->SetCollisionBeginFunction([this, BirdWeakPtr](const FVector3& HitPos, CCollider* Collider) {
			std::shared_ptr<CBird> BirdSharedPtr = BirdWeakPtr.lock();
			if (BirdSharedPtr == nullptr)
			{
				return;
			}
			BirdSharedPtr->Fly(BirdSharedPtr->GetWorldPos() + FVector3(500.f, 500.f, 0.f), [BirdWeakPtr]() {
				BirdWeakPtr.lock()->SetEnable(false);
				});
			BirdSharedPtr->StopTalk();

			int32 Idx = std::rand() % 3;
			mWorld.lock()->GetWorldAssetManager().lock()->SoundPlay("S_BirdFlyUproll" + std::to_string(Idx));

			RemoveTutorial();
			});

#ifdef _DEBUG

		FlyTrigger->SetDebugDraw(true);

#endif // _DEBUG
	}

	AddSubGameObject("Bird", BirdWeakPtr);

	return true;
}

void CStage0StaticTileMap1Object::RollBackTutorial()
{
	if (mTutirialTriggers.empty() == true || mTutorialBird.expired() == true)
	{
		return;
	}

	mTutorialBird.lock()->StopTalk();
}

void CStage0StaticTileMap1Object::RemoveTutorial()
{
	for (auto& Trigger : mTutirialTriggers)
	{
		Trigger.lock()->SetEnable(false);
		Trigger.lock()->Destroy();
	}
	mTutirialTriggers.clear();
}

#pragma endregion

// 2번쩨
#pragma region CStage0StaticTileMap2Object

CStage0StaticTileMap2Object::CStage0StaticTileMap2Object()
{
	SetClassType<CStage0StaticTileMap2Object>();
}

bool CStage0StaticTileMap2Object::Init()
{
	CStaticTileMapObject::Init();

	mPlayerRespawnOffsets[EMapMoveDirection::Left] = FVector3(40.f, 104.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::Right] = FVector3(988.f, 208.f, 0.f);

	if (InitMainTileMap() == false)
	{
		return false;
	}
	if (InitCollisions() == false)
	{
		return false;
	}
	if (InitPlatforms() == false)
	{
		return false;
	}
	if (InitEnvMeshes() == false)
	{
		return false;
	}
	if (InitBlockers() == false)
	{
		return false;
	}
	if (InitNPCs() == false)
	{
		return false;
	}

	return true;
}

void CStage0StaticTileMap2Object::LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData)
{
	CStaticTileMapObject::LoadMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage0StaticTileMap2Object::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData != nullptr)
	{
		if (CurRuntimeData->mTalkOldlady == true)
		{
			RemoveTutorial();
			std::shared_ptr<COldLady> OldLady = mTutorialOldLady.lock();
			if (OldLady != nullptr)
			{
				OldLady->LaughForever();
			}
		}
	}
}

void CStage0StaticTileMap2Object::SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const
{
	CStaticTileMapObject::SaveMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage0StaticTileMap2Object::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData != nullptr)
	{
		CurRuntimeData->mTalkOldlady = mTutirialTriggers.empty();
	}
}

bool CStage0StaticTileMap2Object::InitMainTileMap()
{
	{
		FTileMapLayer& MapLayer = AddTileLayer("Ice");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile0");
		TileRenderComp->EnableTileAlphaBlend();
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Ice", TEXT("tutorial_ice_3.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Dirt");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile1");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Dirt", TEXT("tutorial_rock_3.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Snow");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile2");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Snow", TEXT("tutorial_snow_3.tlm"));
	}

	return true;
}

bool CStage0StaticTileMap2Object::InitCollisions()
{
	static const std::vector<std::pair<FVector2, FVector2>> WallTransforms =
	{
		{{208.f, 78.f},		{104.f, 39.f}},
		{{52.f, 104.f},		{208.f, 52.f}},
		{{52.f, 156.f},		{234.f, 78.f}},

		{{494.f, 234.f},	{507.f, 117.f}},
		{{286.f, 182.f},	{897.f, 91.f}},
	};

	for (auto& WallTransform : WallTransforms)
	{
		std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Wall").lock();

		WallBoxComp->SetCollisionProfile("Static");
		WallBoxComp->SetBoxSize(WallTransform.first);
		WallBoxComp->SetRelativePos(WallTransform.second);
#ifdef _DEBUG

		WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
		WallBoxComp->SetInheritScale(false);
		WallBoxComp->SetStatic(true);
	}

	return true;
}

bool CStage0StaticTileMap2Object::InitPlatforms()
{
	return true;
}

bool CStage0StaticTileMap2Object::InitEnvMeshes()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> EnvObjectSets =
	{
		{{104.f, 78.f},			{52.f, 260.f},		"T_Stage0_Lamp"},

		{{505.375f, 230.75f},	{490.75f, 201.5f},	"T_Stage0_OldladyHouse"},

		{{832.f, 182.f},		{78.f, 78.f},		"T_Stage0_Axe"},
	};

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (auto& EnvObjectSet : EnvObjectSets)
	{
		std::shared_ptr<CMeshComponent> MeshComponent = CreateComponent<CMeshComponent>("EnvObject").lock();
		MeshComponent->SetShader("DefaultTexture2D");
		MeshComponent->SetMesh("RectTex");
		MeshComponent->SetRelativePos(std::get<0>(EnvObjectSet));
		MeshComponent->SetWorldScale(std::get<1>(EnvObjectSet));
		MeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		MeshComponent->SetBlendState(0, "AlphaBlend");
		MeshComponent->SetRenderLayer("Map");
		MeshComponent->SetPivot(0.5f, 0.f);
		MeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture(std::get<2>(EnvObjectSet)));
	}

	return true;
}

bool CStage0StaticTileMap2Object::InitBlockers()
{
	static const std::vector<std::pair<FVector2, FVector2>> BlockerTransforms =
	{
		{{50.f, 260.f},		{-25.f, 520.f}},
		{{1040.f, 50.f},	{520.f, 675.f}},
	};

	for (auto& WallTransform : BlockerTransforms)
	{
		std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Blocker").lock();

		WallBoxComp->SetCollisionProfile("Blocker");
		WallBoxComp->SetBoxSize(WallTransform.first);
		WallBoxComp->SetRelativePos(WallTransform.second);
#ifdef _DEBUG

		WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
		WallBoxComp->SetInheritScale(false);
		WallBoxComp->SetStatic(true);
	}

	std::shared_ptr<CDamageBoxCollider> FloorBoxComp = CreateComponent<CDamageBoxCollider>("FloorBlocker").lock();
	FloorBoxComp->SetBoxSize(1040.f, 50.f);
	FloorBoxComp->SetRelativePos(520.f, -40.f);

	return true;
}

bool CStage0StaticTileMap2Object::InitNPCs()
{
	std::weak_ptr<COldLady> OldLadyWeakPtr = mWorld.lock()->CreateGameObject<COldLady>("OldLady");
	mTutorialOldLady = OldLadyWeakPtr;

	std::shared_ptr<COldLady> OldLadySharedPtr = OldLadyWeakPtr.lock();
	OldLadySharedPtr->SetWorldPos(GetWorldPos() + FVector3(624.f, 234.f, 0.f));
	OldLadySharedPtr->SetSightDir(-FVector3::Axis[EAxis::X]);

	std::shared_ptr<FDialogueContext> TutorialContext = std::make_shared<FDialogueContext>();
	{
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Player;
			Data.mText = TEXT("실례합니다?");
			Data.mStartEmotionName = "Normal";
			Data.mLoopEmotionName = "NormalLoop";
			Data.mOnEnd = [this]() {
				auto TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
				auto TargetCharacter = std::static_pointer_cast<CCharacter>(TileWorld->GetTargetObject().lock());
				TargetCharacter->GetMovementComp()->SetApplyMove(true);
				TargetCharacter->GetMovementComp()->MoveStraight(TargetCharacter->GetWorldPos() + FVector3(182.f, 0.f, 0.f));
				TargetCharacter->GetMovementComp()->SetApplyMove(false);
				};

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Player;
			Data.mText = TEXT("앞에 표지판이 부서졌는데...\n여기가 산 등산로인가요?");
			Data.mStartEmotionName = "Normal";
			Data.mLoopEmotionName = "NormalLoop";

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Other;
			Data.mText = TEXT("거의 다 왔어.\n다리만 건너면 돼.");
			Data.mStartEmotionName = "Normal";
			Data.mLoopEmotionName = "NormalLoop";
			Data.mOnEnd = [this, OldLadyWeakPtr]() {
				auto TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
				auto TargetPlayer = std::static_pointer_cast<CPlayer>(TileWorld->GetTargetObject().lock());
				TargetPlayer->GetMovementComp()->SetApplyMove(true);
				TargetPlayer->GetMovementComp()->MoveStraight(TargetPlayer->GetWorldPos() + FVector3(182.f, 0.f, 0.f));
				TargetPlayer->GetMovementComp()->SetSight(-FVector3::Axis[EAxis::X]);
				TargetPlayer->GetMovementComp()->SetApplyMove(false);

				std::shared_ptr<COldLady> OldLadySharedPtr = OldLadyWeakPtr.lock();
				OldLadySharedPtr->ChangeCamera(
					TargetPlayer->GetCameraComp()->GetWorldPos(),
					TargetPlayer->GetCameraComp()->GetZoomRate(),
					OldLadySharedPtr->GetWorldPos(),
					0.5f
					);
				};

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Player;
			Data.mText = TEXT("그런데, 차도 수리하러 사람 불러야겠는데요. 마룻대가 무너져서 죽을 뻔했어요.");
			Data.mStartEmotionName = "Sad";
			Data.mLoopEmotionName = "SadLoop";
			Data.mOnEnd = [OldLadyWeakPtr]() {
				std::shared_ptr<COldLady> OldLadySharedPtr = OldLadyWeakPtr.lock();
				OldLadySharedPtr->LaughForever();
				};

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Other;
			Data.mText = TEXT("내 \"차도\" 때문에 죽을 뻔했다면\n산을 어떻게 감담하냐.");
			Data.mLoopEmotionName = "LaughLoop";

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Player;
			Data.mText = TEXT("...");
			Data.mLoopEmotionName = "UpsetLoop";

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Player;
			Data.mText = TEXT("늙은 박쥐 같은 할머니가\n여기에서 살 수 있다면 나도 괜찮을 것이라고 생각해요.");
			Data.mStartEmotionName = "Angry";
			Data.mLoopEmotionName = "AngryLoop";
			Data.mOnEnd = [OldLadyWeakPtr]() {
				std::shared_ptr<COldLady> OldLadySharedPtr = OldLadyWeakPtr.lock();
				OldLadySharedPtr->StopLaugh();
				};

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Other;
			Data.mText = TEXT("알아서 해라.");
			Data.mStartEmotionName = "Normal";
			Data.mLoopEmotionName = "NormalLoop";
			Data.mOnEnd = [this, OldLadyWeakPtr]() {
				auto TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
				auto TargetPlayer = std::static_pointer_cast<CPlayer>(TileWorld->GetTargetObject().lock());

				std::shared_ptr<COldLady> OldLadySharedPtr = OldLadyWeakPtr.lock();
				OldLadySharedPtr->ChangeCamera(
					(TargetPlayer->GetWorldPos() + OldLadySharedPtr->GetWorldPos()) * 0.5f,
					0.35f
				);
				};

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Other;
			Data.mText = TEXT("하지만 말이야,\n셀레스트 산은 이상한 곳이야.");
			Data.mStartEmotionName = "Normal";
			Data.mLoopEmotionName = "NormalLoop";

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Other;
			Data.mText = TEXT("헛것이 보일지도 몰라.");
			Data.mStartEmotionName = "CreepA";
			Data.mLoopEmotionName = "CreepALoop";

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Other;
			Data.mText = TEXT("볼 준비가 안 된 것들 말이야");
			Data.mStartEmotionName = "CreepB";
			Data.mLoopEmotionName = "CreepBLoop";
			Data.mOnEnd = [this, OldLadyWeakPtr]() {
				auto TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
				auto TargetPlayer = std::static_pointer_cast<CPlayer>(TileWorld->GetTargetObject().lock());

				std::shared_ptr<COldLady> OldLadySharedPtr = OldLadyWeakPtr.lock();
				OldLadySharedPtr->LaughForever();
				OldLadySharedPtr->ChangeCamera(
					(TargetPlayer->GetWorldPos() + OldLadySharedPtr->GetWorldPos()) * 0.5f,
					0.5f
				);
				};

			TutorialContext->mDialogueDatas.push_back(Data);
		}
		{
			FDialogueData Data;
			Data.mTurn = EDialogueTurn::Player;
			Data.mText = TEXT("병원에 가보세요, 할머니.");
			Data.mStartEmotionName = "Upset";
			Data.mLoopEmotionName = "UpsetLoop";
			Data.mOnEnd = [this, OldLadyWeakPtr]() {
				auto TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
				auto TargetPlayer = std::static_pointer_cast<CPlayer>(TileWorld->GetTargetObject().lock());

				std::shared_ptr<COldLady> OldLadySharedPtr = OldLadyWeakPtr.lock();
				OldLadySharedPtr->LaughForever();
				OldLadySharedPtr->RollBackCamera(
					std::static_pointer_cast<CTileMapCameraComponent>(TargetPlayer->GetCameraComp()),
					1.f
				);
				};

			TutorialContext->mDialogueDatas.push_back(Data);
		}
	}
	OldLadySharedPtr->SetDialogueContext(TutorialContext);

	std::shared_ptr<CColliderBox2D> DialogueTrigger = CreateComponent<CColliderBox2D>("DialogueTrigger").lock();
	{
		mTutirialTriggers.push_back(DialogueTrigger);

		DialogueTrigger->SetRelativePos(416.f, 286.f);
		DialogueTrigger->SetBoxSize(104.f, 104.f);
		DialogueTrigger->SetCollisionProfile("EventTrigger");
		DialogueTrigger->SetCollisionBeginFunction([this, OldLadyWeakPtr](const FVector3& HitPos, CCollider* Collider) {
			auto Target = std::static_pointer_cast<CCharacter>(Collider->GetOwner().lock());
			if (Target == nullptr)
			{
				return;
			}

			FPlayerGameState& PlayerGameState = static_cast<FPlayerGameState&>(Target->GetMovementComp()->GetSharedState());
			PlayerGameState.AddConversableObject(OldLadyWeakPtr);
			Target->GetMovementComp()->SetApplyMove(true);
			Target->GetMovementComp()->DoMovement("Dialogue");
			Target->GetMovementComp()->SetApplyMove(false);
			PlayerGameState.RemoveConversableObject(OldLadyWeakPtr);

			RemoveTutorial();
			});

#ifdef _DEBUG

		DialogueTrigger->SetDebugDraw(true);

#endif // _DEBUG
	}

	AddSubGameObject("OldLady", OldLadyWeakPtr);

	return true;
}

void CStage0StaticTileMap2Object::RemoveTutorial()
{
	for (auto& Trigger : mTutirialTriggers)
	{
		Trigger.lock()->SetEnable(false);
		Trigger.lock()->Destroy();
	}
	mTutirialTriggers.clear();
}

#pragma endregion

// 3번쩨
#pragma region CStage0StaticTileMap3Object

CStage0StaticTileMap3Object::CStage0StaticTileMap3Object()
{
	SetClassType<CStage0StaticTileMap3Object>();
}

bool CStage0StaticTileMap3Object::Init()
{
	CStaticTileMapObject::Init();

	mPlayerRespawnOffsets[EMapMoveDirection::Left] = FVector3(104.f, 201.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::Right] = FVector3(2990.f, 201.f, 0.f);

	if (InitMainTileMap() == false)
	{
		return false;
	}
	if (InitCollisions() == false)
	{
		return false;
	}
	if (InitPlatforms() == false)
	{
		return false;
	}
	if (InitEnvMeshes() == false)
	{
		return false;
	}
	if (InitBlockers() == false)
	{
		return false;
	}
	if (InitNPCs() == false)
	{
		return false;
	}
	if (InitDynamicObjects() == false)
	{
		return false;
	}

	return true;
}

void CStage0StaticTileMap3Object::LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData)
{
	CStaticTileMapObject::LoadMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage0StaticTileMap3Object::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData != nullptr)
	{
		for (int32 i = 0; i < mAllBridgeBlockCount; ++i)
		{
			auto BridgeBlock = mBridgeBlocks[i].lock();
			BridgeBlock->LoadMapRuntimeData(CurRuntimeData->mBridgeBlockDatas[i]);
		}

		std::shared_ptr<CColliderBox2D> BGMTrigger = mBGMTrigger.lock();
		if (BGMTrigger != nullptr)
		{
			BGMTrigger->SetEnable(true);

			std::shared_ptr<CWorld> World = mWorld.lock();
			std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();
			if (WorldAssetMgr->IsSoundPlaying("S_Stage0BrdigeBGM") == true)
			{
				WorldAssetMgr->SoundStop("S_Stage0BrdigeBGM", true, 1.f);
			}
			if (WorldAssetMgr->IsSoundPlaying("S_Stage0MainBGM") == false)
			{
				WorldAssetMgr->SoundPlay("S_Stage0MainBGM", true, 1.f);
			}
			if (WorldAssetMgr->IsSoundPlaying("S_Stage0BridgeRumble") == true)
			{
				WorldAssetMgr->SoundStop("S_Stage0BridgeRumble", true, 1.f);
			}
		}

		std::shared_ptr<CWorldTimeChangableBoxCollider> SlowTrigger = mSlowTrigger.lock();
		if (SlowTrigger != nullptr)
		{
			SlowTrigger->ResetTrigger();
		}

		std::shared_ptr<CBird> Bird = mTutorialBird.lock();
		if (Bird != nullptr)
		{
			Bird->SetWorldPos(GetWorldPos() + FVector3(3042.f, 700.f, 0.f));
			Bird->StopTalk();
		}
	}
}

void CStage0StaticTileMap3Object::SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const
{
	CStaticTileMapObject::SaveMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage0StaticTileMap3Object::FMapRuntimeData>(RuntimeData);
}

bool CStage0StaticTileMap3Object::InitMainTileMap()
{
	{
		FTileMapLayer& MapLayer = AddTileLayer("Ice");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile0");
		TileRenderComp->EnableTileAlphaBlend();
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Ice", TEXT("tutorial_ice_4.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Dirt");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile1");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Dirt", TEXT("tutorial_rock_4.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Snow");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile2");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Snow", TEXT("tutorial_snow_4.tlm"));
	}

	return true;
}

bool CStage0StaticTileMap3Object::InitCollisions()
{
	static const std::vector<std::pair<FVector2, FVector2>> WallTransforms =
	{
		{{130.f, 182.f},	{65.f, 91.f}},
		{{26.f, 130.f},		{143.f, 117.f}},
		{{52.f, 52.f},		{156.f, 130.f}},
		{{208.f, 26.f},		{104.f, 169.f}},

		{{156.f, 78.f},		{3146.f, 39.f}},
		{{208.f, 52.f},		{3120.f, 78.f}},
		{{286.f, 104.f},	{3081.f, 130.f}},
		{{78.f, 208.f},		{3185.f, 104.f}},
		{{52.f, 260.f},		{3198.f, 130.f}},
		{{26.f, 338.f},		{3211.f, 169.f}},
	};

	for (auto& WallTransform : WallTransforms)
	{
		std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Wall").lock();

		WallBoxComp->SetCollisionProfile("Static");
		WallBoxComp->SetBoxSize(WallTransform.first);
		WallBoxComp->SetRelativePos(WallTransform.second);
#ifdef _DEBUG

		WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
		WallBoxComp->SetInheritScale(false);
		WallBoxComp->SetStatic(true);
	}

	return true;
}

bool CStage0StaticTileMap3Object::InitPlatforms()
{
	return true;
}

bool CStage0StaticTileMap3Object::InitEnvMeshes()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> EnvObjectSets =
	{
		{{3146.f, 182.f},		{52.f, 260.f},		"T_Stage0_LampGrass"},
	};

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (auto& EnvObjectSet : EnvObjectSets)
	{
		std::shared_ptr<CMeshComponent> MeshComponent = CreateComponent<CMeshComponent>("EnvObject").lock();
		MeshComponent->SetShader("DefaultTexture2D");
		MeshComponent->SetMesh("RectTex");
		MeshComponent->SetRelativePos(std::get<0>(EnvObjectSet));
		MeshComponent->SetWorldScale(std::get<1>(EnvObjectSet));
		MeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		MeshComponent->SetBlendState(0, "AlphaBlend");
		MeshComponent->SetRenderLayer("Map");
		MeshComponent->SetPivot(0.5f, 0.f);
		MeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture(std::get<2>(EnvObjectSet)));
	}

	return true;
}

bool CStage0StaticTileMap3Object::InitBlockers()
{
	static const std::vector<std::pair<FVector2, FVector2>> BlockerTransforms =
	{
		{{50.f, 650.f},		{3251.f, 325.f}},
		{{3224.f, 50.f},	{1612.f, 675.f}},
	};

	for (auto& WallTransform : BlockerTransforms)
	{
		std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Blocker").lock();

		WallBoxComp->SetCollisionProfile("Blocker");
		WallBoxComp->SetBoxSize(WallTransform.first);
		WallBoxComp->SetRelativePos(WallTransform.second);
#ifdef _DEBUG

		WallBoxComp->SetDebugDraw(true);

#endif // DEBUG
		WallBoxComp->SetInheritScale(false);
		WallBoxComp->SetStatic(true);
	}

	std::shared_ptr<CDamageBoxCollider> FloorBoxComp = CreateComponent<CDamageBoxCollider>("FloorBlocker").lock();
	FloorBoxComp->SetBoxSize(3224.f, 50.f);
	FloorBoxComp->SetRelativePos(1612.f, -40.f);

	return true;
}

bool CStage0StaticTileMap3Object::InitDynamicObjects()
{
	static std::unordered_set<int32> EmptyBlockSet = {
		45,
		46,
		47,
		48,
		89,
		90,
		91,
		92,
		93,
		94,
		95,
		96,
		97,
		98
	};
	static std::unordered_map<int32, std::pair<float, float>> TriggerMap = {
		{0, {338.f, 0.3f}},
		{1, {286.f, 0.f}},
		{2, {260.f, 0.4f}},
		{3, {234.f, 0.3f}},
		{4, {208.f, 0.3f}},
		{5, {182.f, 0.f}},
		{6, {156.f, 0.3f}},
		{7, {130.f, 0.3f}},
		{8, {104.f, 0.2f}},
		{9, {78.f, 0.3f}},
		{10, {26.f, 0.1f}},

		{77, {-364.f, 0.f}},
		{78, {-364.f, 0.f}},
		{79, {-364.f, 0.f}},
		{80, {-364.f, 0.f}},
		{81, {-364.f, 0.f}},

		{82, {-260.f, 0.f}},
		{83, {-260.f, 0.f}},
		{84, {-260.f, 0.f}},
		{85, {-260.f, 0.f}},
		{86, {-260.f, 0.f}},
		{87, {-260.f, 0.f}},
		{88, {-260.f, 0.f}},
	};
	static std::array<FVector3, mOneBridgeBlockCount> BlockCollisionSizes = {
		FVector3(52.f, 52.f, 1.f),
		FVector3(26.f, 52.f, 1.f),
		FVector3(26.f, 26.f, 1.f),
		FVector3(26.f, 26.f, 1.f),
		FVector3(26.f, 26.f, 1.f),
		FVector3(26.f, 26.f, 1.f),
		FVector3(26.f, 26.f, 1.f),
		FVector3(26.f, 26.f, 1.f),
		FVector3(26.f, 52.f, 1.f),
		FVector3(52.f, 52.f, 1.f),
		FVector3(26.f, 78.f, 1.f),
	};
	static std::array<FVector3, mOneBridgeBlockCount> BlockMeshSizes = {
		FVector3(52.f, 195.f, 1.f),
		FVector3(26.f, 195.f, 1.f),
		FVector3(26.f, 195.f, 1.f),
		FVector3(26.f, 195.f, 1.f),
		FVector3(26.f, 195.f, 1.f),
		FVector3(26.f, 195.f, 1.f),
		FVector3(26.f, 195.f, 1.f),
		FVector3(26.f, 195.f, 1.f),
		FVector3(26.f, 195.f, 1.f),
		FVector3(52.f, 195.f, 1.f),
		FVector3(26.f, 195.f, 1.f),
	};

	int32 AccBlockNumber = 0;
	float AccBlockPosX = 208.f;
	const int32 BridgeCount = 9;
	const int32 BridgeBlockCount = mOneBridgeBlockCount * 9;
	for (int32 i = 0; i < BridgeBlockCount; ++i)
	{
		const int32 BlockIndex = i % mOneBridgeBlockCount;
		if (EmptyBlockSet.find(i) != EmptyBlockSet.end())
		{
			AccBlockPosX += BlockCollisionSizes[BlockIndex].x;
			continue;
		}

		FTriggerOneTimeDropObjectInitInfo InitInfo;

		InitInfo.mWorldPos = FVector3(AccBlockPosX, 182.f - BlockCollisionSizes[BlockIndex].y, 0.f);
		InitInfo.mMeshWorldPos = FVector3(AccBlockPosX, 13.f, 0.f);
		InitInfo.mMeshScale = BlockMeshSizes[BlockIndex];
		InitInfo.mRenderLayerName = "FrontObject";
		InitInfo.mIsEnableAlpha = true;

		InitInfo.mCollisionSize = { BlockCollisionSizes[BlockIndex].x, BlockCollisionSizes[BlockIndex].y };

		InitInfo.mTextureName = "T_Stage0_BridgeBlock" + std::to_string(BlockIndex);

		InitInfo.mDropGravityScale = 2.f;
		InitInfo.mDropReadyMaxTime = 0.4f;

		InitInfo.mLifeTime = 0.5f;

		InitInfo.mTriggerBoxWorldPos = FVector3(AccBlockPosX, 182.f, 0.f);
		InitInfo.mTriggerBoxSize = { 26.f, 390.f };
		InitInfo.mOnlyDropByTrigger = true;

		if (TriggerMap.find(i) != TriggerMap.end())
		{
			InitInfo.mTriggerBoxWorldPos.x += TriggerMap[i].first;
			InitInfo.mTriggerDelayTime = TriggerMap[i].second;
		}

		std::shared_ptr<CTriggerOneTimeDropMapObject> DropObject = CTriggerOneTimeDropMapObject::MakeQuikly(mWorld.lock(), InitInfo);
		AddSubGameObject("BridgeBlock" + std::to_string(AccBlockNumber), DropObject);
		mBridgeBlocks[AccBlockNumber] = DropObject;

		++AccBlockNumber;
		AccBlockPosX += BlockCollisionSizes[BlockIndex].x;
	}

	return true;
}

bool CStage0StaticTileMap3Object::InitNPCs()
{
	std::weak_ptr<CColliderBox2D> BGMTriggerWeakPtr = CreateComponent<CColliderBox2D>("BGMTrigger");
	std::shared_ptr<CColliderBox2D> BGMTriggerSharedPtr = BGMTriggerWeakPtr.lock();
	{
		mBGMTrigger = BGMTriggerSharedPtr;

		BGMTriggerSharedPtr->SetRelativePos(546.f, 377.f);
		BGMTriggerSharedPtr->SetBoxSize(26.f, 390.f);
		BGMTriggerSharedPtr->SetCollisionProfile("EventTrigger");
		BGMTriggerSharedPtr->SetCollisionBeginFunction([BGMTriggerWeakPtr](const FVector3& HitPos, CCollider* Collider) {
			std::shared_ptr<CColliderBox2D> BGMTriggerSharedPtr = BGMTriggerWeakPtr.lock();
			BGMTriggerSharedPtr->SetEnable(false);

			std::shared_ptr<CWorld> World = BGMTriggerSharedPtr->GetWorld().lock();
			std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

			if (WorldAssetMgr->IsSoundPlaying("S_Stage0MainBGM") == true)
			{
				WorldAssetMgr->SoundStop("S_Stage0MainBGM", true, 1.f);
			}
			WorldAssetMgr->SoundPlay("S_Stage0BrdigeBGM", true, 1.f);
			WorldAssetMgr->SoundPlay("S_Stage0BridgeRumble", true, 1.f);
			});

#ifdef _DEBUG

		BGMTriggerSharedPtr->SetDebugDraw(true);

#endif // _DEBUG
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::weak_ptr<CBird> BirdWeakPtr = World->CreateGameObject<CBird>("Bird");
	mTutorialBird = BirdWeakPtr;

	std::shared_ptr<CBird> BirdSharedPtr = BirdWeakPtr.lock();
	BirdSharedPtr->SetWorldPos(GetWorldPos() + FVector3(3042.f, 700.f, 0.f));
	BirdSharedPtr->SetSightDir(-FVector3::Axis[EAxis::X]);
	BirdSharedPtr->SetUseWorldDeltaTime(false);

	std::weak_ptr<CWorldTimeChangableBoxCollider> SlowTriggerWeakPtr = CreateComponent<CWorldTimeChangableBoxCollider>("SlowTrigger");
	std::shared_ptr<CWorldTimeChangableBoxCollider> SlowTriggerSharedPtr = SlowTriggerWeakPtr.lock();
	{
		mSlowTrigger = SlowTriggerWeakPtr;

		SlowTriggerSharedPtr->SetRelativePos(2626.f, 280.f);
		SlowTriggerSharedPtr->SetBoxSize(26.f, 196.f);
		SlowTriggerSharedPtr->SetCollisionProfile("EventTrigger");
		SlowTriggerSharedPtr->SetOneTimeTrigger(true);
		SlowTriggerSharedPtr->SetTargetWorldTimeRate(0.f);
		SlowTriggerSharedPtr->SetWorldTimeChangeMaxTime(1.f);
		SlowTriggerSharedPtr->mOnStartLerpWorldTime.push_back([this]() {
			std::shared_ptr<CWorld> World = mWorld.lock();
			std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

			if (WorldAssetMgr->IsSoundPlaying("S_Stage0BridgeRumble") == true)
			{
				WorldAssetMgr->SoundStop("S_Stage0BridgeRumble", true, 1.f);
			}
			});
		SlowTriggerSharedPtr->mOnChangeTargetWorldTime.push_back(([this, BirdWeakPtr, SlowTriggerWeakPtr]() {
			std::shared_ptr<CBird> BirdSharedPtr = BirdWeakPtr.lock();
			if (BirdSharedPtr == nullptr)
			{
				return;
			}
			BirdSharedPtr->Fly(GetWorldPos() + FVector3(2990.f, 202.f, 0.f), [this, BirdWeakPtr, SlowTriggerWeakPtr]() {
				std::shared_ptr<CBird> BirdSharedPtr = BirdWeakPtr.lock();
				if (BirdSharedPtr == nullptr)
				{
					return;
				}
				BirdSharedPtr->Cry();
				BirdSharedPtr->StartTalk(TEXT("대시"), TEXT("방향키와"), "T_InputZ");

				auto TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
				auto TargetTutorialPlayer = std::static_pointer_cast<CTutorialPlayer>(TileWorld->GetTargetObject().lock());
				TargetTutorialPlayer->GrantDash([SlowTriggerWeakPtr]() {
					SlowTriggerWeakPtr.lock()->ResetWorldTimeRate();
					});
				});
			}));

#ifdef _DEBUG

		SlowTriggerSharedPtr->SetDebugDraw(true);

#endif // _DEBUG
	}

	std::shared_ptr<CColliderBox2D> FlyTrigger = CreateComponent<CColliderBox2D>("BirdFlyTrigger").lock();
	{
		mTutirialTrigger = FlyTrigger;

		FlyTrigger->SetRelativePos(3042.f, 234.f);
		FlyTrigger->SetBoxSize(104.f, 104.f);
		FlyTrigger->SetCollisionProfile("EventTrigger");
		FlyTrigger->SetCollisionBeginFunction([this, BirdWeakPtr](const FVector3& HitPos, CCollider* Collider) {
			auto TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
			auto TargetCharacter = std::static_pointer_cast<CCharacter>(TileWorld->GetTargetObject().lock());
			if (TargetCharacter == nullptr)
			{
				return;
			}
			TargetCharacter->GetMovementComp()->SetApplyMove(false);

			std::shared_ptr<CBird> BirdSharedPtr = BirdWeakPtr.lock();
			if (BirdSharedPtr == nullptr)
			{
				return;
			}
			BirdSharedPtr->Fly(BirdSharedPtr->GetWorldPos() + FVector3(500.f, 500.f, 0.f));
			BirdSharedPtr->StopTalk();

			mTutirialTrigger.lock()->Destroy();
			mTutirialTrigger.reset();

			TileWorld->GetTileMapFactory().lock()->StartEnding();

			int32 Idx = std::rand() % 3;
			TileWorld->GetWorldAssetManager().lock()->SoundPlay("S_BirdFlyUproll" + std::to_string(Idx));
			});

#ifdef _DEBUG

		FlyTrigger->SetDebugDraw(true);

#endif // _DEBUG
	}

	AddSubGameObject("Bird", BirdWeakPtr);

	return true;
}

#pragma endregion