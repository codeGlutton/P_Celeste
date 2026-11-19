#include "Pch.h"
#include "DummyStaticTileMapObject.h"

#include "Component/TileMapRender.h"
#include "Component/TileMapComponent.h"
#include "Component/ColliderBox2D.h"
#include "Component/ColliderOneWayBox2D.h"
#include "Component/MeshComponent.h"

#include "World/World.h"
#include "World/WorldAssetManager.h"

CDummyStaticTileMapObject::CDummyStaticTileMapObject()
{
	SetClassType<CDummyStaticTileMapObject>();
}

bool CDummyStaticTileMapObject::Init()
{
	CStaticTileMapObject::Init();

	// 8방향 모든 진입·리스폰 방향에 대해 안전한 바닥 좌표 지정
	mPlayerRespawnOffsets[EMapMoveDirection::Left] = FVector3(130.f, 150.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::Right] = FVector3(1430.f, 150.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::Up] = FVector3(780.f, 520.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::Down] = FVector3(780.f, 150.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::UpRight] = FVector3(1430.f, 150.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::UpLeft] = FVector3(130.f, 150.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::DownRight] = FVector3(1430.f, 150.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::DownLeft] = FVector3(130.f, 150.f, 0.f);

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
	if (InitDynamicObjects() == false)
	{
		return false;
	}

	return true;
}

void CDummyStaticTileMapObject::LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData)
{
	CStaticTileMapObject::LoadMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CDummyStaticTileMapObject::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData == nullptr)
	{
		return;
	}

	for (int32 i = 0; i < mDynamicBlockCount; ++i)
	{
		std::shared_ptr<CTriggerOneTimeDropMapObject> Block = mDynamicBlocks[i].lock();
		if (Block != nullptr)
		{
			Block->LoadMapRuntimeData(CurRuntimeData->mBlockDatas[i]);
		}
	}
}

void CDummyStaticTileMapObject::SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const
{
	CStaticTileMapObject::SaveMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CDummyStaticTileMapObject::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData == nullptr)
	{
		return;
	}

	for (int32 i = 0; i < mDynamicBlockCount; ++i)
	{
		std::shared_ptr<const CTriggerOneTimeDropMapObject> Block = mDynamicBlocks[i].lock();
		if (Block != nullptr)
		{
			Block->SaveMapRuntimeData(CurRuntimeData->mBlockDatas[i]);
		}
	}
}

bool CDummyStaticTileMapObject::InitMainTileMap()
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

bool CDummyStaticTileMapObject::InitCollisions()
{
	// 좌우 구역 이동이 자유로운 바닥 + 상하 이동용 중앙 개구부 양옆 천장/발판 구성
	static const std::vector<std::pair<FVector2, FVector2>> WallTransforms =
	{
		{{624.f, 104.f},	{312.f, 52.f}},
		{{624.f, 104.f},	{1248.f, 52.f}},
		{{260.f, 52.f},		{260.f, 260.f}},
		{{260.f, 52.f},		{1300.f, 260.f}},
		{{208.f, 52.f},		{416.f, 442.f}},
		{{208.f, 52.f},		{1144.f, 442.f}},
		{{520.f, 52.f},		{260.f, 624.f}},
		{{520.f, 52.f},		{1300.f, 624.f}},
	};

	for (const auto& WallTransform : WallTransforms)
	{
		std::shared_ptr<CColliderBox2D> WallBoxComp = CreateComponent<CColliderBox2D>("Wall").lock();

		WallBoxComp->SetCollisionProfile("Static");
		WallBoxComp->SetBoxSize(WallTransform.first);
		WallBoxComp->SetRelativePos(WallTransform.second);
#ifdef _DEBUG
		WallBoxComp->SetDebugDraw(true);
#endif // _DEBUG
		WallBoxComp->SetInheritScale(false);
		WallBoxComp->SetStatic(true);
	}

	return true;
}

bool CDummyStaticTileMapObject::InitPlatforms()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> PlatformObjectSets =
	{
		{{624.f, 78.f},		{156.f, 26.f},	"T_Stage0_WoodPlatformLeft_2x1"},
		{{780.f, 78.f},		{156.f, 26.f},	"T_Stage0_WoodPlatformRight_2x1"},
		{{702.f, 260.f},	{156.f, 26.f},	"T_Stage0_WoodPlatformLeft_2x1"},
		{{702.f, 442.f},	{156.f, 26.f},	"T_Stage0_WoodPlatformRight_2x1"},
	};

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (const auto& PlatformObjectSet : PlatformObjectSets)
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

		const FVector2 PlatformCollisionPos = {
			std::get<0>(PlatformObjectSet).x + std::get<1>(PlatformObjectSet).x * 0.5f,
			std::get<0>(PlatformObjectSet).y + std::get<1>(PlatformObjectSet).y - 2.5f
		};

		OneWayBoxComp->SetCollisionProfile("Static");
		OneWayBoxComp->SetOneWayAxis(FVector3::Axis[EAxis::Y]);
		OneWayBoxComp->SetRelativePos(PlatformCollisionPos);
		OneWayBoxComp->SetBoxSize(std::get<1>(PlatformObjectSet).x, 5.f);
#ifdef _DEBUG
		OneWayBoxComp->SetDebugDraw(true);
#endif // _DEBUG
		OneWayBoxComp->SetInheritScale(false);
		OneWayBoxComp->SetStatic(true);
	}

	return true;
}

bool CDummyStaticTileMapObject::InitEnvMeshes()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> EnvObjectSets =
	{
		{{208.f, 104.f},	{52.f, 260.f},	"T_Stage0_Lamp"},
		{{1352.f, 104.f},	{52.f, 260.f},	"T_Stage0_LampGrass"},
		{{312.f, 104.f},	{52.f, 52.f},	"T_Stage0_Fence0"},
		{{364.f, 104.f},	{52.f, 52.f},	"T_Stage0_FenceMiddle0"},
		{{1196.f, 104.f},	{52.f, 52.f},	"T_Stage0_FenceMiddle0"},
		{{1248.f, 104.f},	{52.f, 52.f},	"T_Stage0_Sign_RightArrow"},
	};

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (const auto& EnvObjectSet : EnvObjectSets)
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

bool CDummyStaticTileMapObject::InitDynamicObjects()
{
	for (int32 i = 0; i < mDynamicBlockCount; ++i)
	{
		const float PosX = 260.f + i * 130.f;

		FTriggerOneTimeDropObjectInitInfo InitInfo;
		InitInfo.mWorldPos = FVector3(PosX, 364.f, 0.f);
		InitInfo.mMeshWorldPos = FVector3(PosX, 234.f, 0.f);
		InitInfo.mMeshScale = FVector3(52.f, 130.f, 1.f);
		InitInfo.mRenderLayerName = "FrontObject";
		InitInfo.mIsEnableAlpha = true;
		InitInfo.mCollisionSize = { 52.f, 26.f };
		InitInfo.mTextureName = "T_Stage0_BridgeBlock" + std::to_string(i % 11);
		InitInfo.mDropGravityScale = 2.f;
		InitInfo.mDropReadyMaxTime = 0.4f;
		InitInfo.mLifeTime = 0.5f;
		InitInfo.mTriggerBoxWorldPos = FVector3(PosX, 390.f, 0.f);
		InitInfo.mTriggerBoxSize = { 26.f, 52.f };
		InitInfo.mOnlyDropByTrigger = false;

		std::shared_ptr<CTriggerOneTimeDropMapObject> DropObject = CTriggerOneTimeDropMapObject::MakeQuikly(mWorld.lock(), InitInfo);
		AddSubGameObject("DummyDropBlock" + std::to_string(i), DropObject);
		mDynamicBlocks[i] = DropObject;
	}

	return true;
}
