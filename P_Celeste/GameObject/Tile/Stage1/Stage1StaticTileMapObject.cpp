#include "Pch.h"
#include "Stage1StaticTileMapObject.h"

#include "Component/TileMapRender.h"
#include "Component/TileMapComponent.h"
#include "Component/ColliderBox2D.h"
#include "Component/ColliderOneWayBox2D.h"
#include "Component/DamageBoxCollider.h"
#include "Component/MeshComponent.h"

// 0번째
#pragma region CStage1StaticTileMap0Object

CStage1StaticTileMap0Object::CStage1StaticTileMap0Object()
{
	SetClassType<CStage1StaticTileMap0Object>();
}

bool CStage1StaticTileMap0Object::Init()
{
	CStaticTileMapObject::Init();

	mPlayerRespawnOffsets[EMapMoveDirection::Left] = FVector3(78.f, 150.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::Up] = FVector3(819.f, 436.f, 0.f);

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

void CStage1StaticTileMap0Object::LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData)
{
	CStaticTileMapObject::LoadMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage1StaticTileMap0Object::FMapRuntimeData>(RuntimeData);
}

void CStage1StaticTileMap0Object::SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const
{
	CStaticTileMapObject::SaveMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage1StaticTileMap0Object::FMapRuntimeData>(RuntimeData);
}

bool CStage1StaticTileMap0Object::InitMainTileMap()
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
		LoadMap("Ice", TEXT("stage1_ice.tlm"));
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
		LoadMap("Dirt", TEXT("stage1_rock.tlm"));
	}
	{
		FTileMapLayer& MapLayer = AddTileLayer("Cement");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile1");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Cement", TEXT("stage1_cement.tlm"));
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
		LoadMap("Snow", TEXT("stage1_snow.tlm"));
	}
	{
		FTileMapLayer& MapLayer = AddTileLayer("Girder");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile2");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Girder", TEXT("stage1_girder.tlm"));
	}

	return true;
}

bool CStage1StaticTileMap0Object::InitCollisions()
{
	static const std::vector<std::pair<FVector2, FVector2>> WallTransforms =
	{
		{{52.f, 234.f},			{26.f, 481.f}},
		{{104.f, 52.f},			{52.f, 572.f}},
		{{130.f, 182.f},		{169.f, 507.f}},
		{{806.f, 78.f},			{403.f, 611.f}},
		{{78.f, 156.f},			{377.f, 520.f}},
		{{52.f, 78.f},			{442.f, 559.f}},
		{{78.f, 52.f},			{507.f, 572.f}},
		{{26.f, 104.f},			{559.f, 546.f}},

		{{26.f, 286.f},			{13.f, 247.f}},
		{{130.f, 130.f},		{65.f, 65.f}},
		{{130.f, 52.f},			{195.f, 26.f}},
		{{78.f, 182.f},			{299.f, 91.f}},
		{{130.f, 104.f},		{403.f, 52.f}},
		{{104.f, 338.f},		{520.f, 169.f}},
		{{104.f, 182.f},		{624.f, 91.f}},
		{{78.f, 78.f},			{715.f, 39.f}},

		{{104.f, 468.f},		{988.f, 416.f}},
		{{130.f, 130.f},		{975.f, 481.f}},
		{{156.f, 26.f},			{962.f, 481.f}},
		{{234.f, 78.f},			{923.f, 377.f}},
		{{156.f, 52.f},			{962.f, 312.f}},
		{{78.f, 52.f},			{1001.f, 156.f}},
		{{52.f, 26.f},			{1014.f, 117.f}},
		{{26.f, 104.f},			{1027.f, 52.f}},
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

bool CStage1StaticTileMap0Object::InitPlatforms()
{
	return true;
}

bool CStage1StaticTileMap0Object::InitEnvMeshes()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> EnvObjectSets =
	{
		{{299.f, 182.f},	{104.f, 162.5f},	"T_Stage1_SignConstructionPanel" },
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

bool CStage1StaticTileMap0Object::InitBlockers()
{
	static const std::vector<std::pair<FVector2, FVector2>> BlockerTransforms =
	{
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

	static const std::vector<std::pair<FVector2, FVector2>> DangerTransforms =
	{
		{{1040.f, 50.f},		{520.f, -40.f}},

		{{130.f, 20.f},			{195.f, 62.f}},
		{{130.f, 20.f},			{403.f, 114.f}},
		{{104.f, 20.f},			{624.f, 192.f}},
		{{78.f, 20.f},			{715.f, 88.f}},

		{{20.f, 52.f},			{874.f, 312.f}},
	};

	for (auto& DangerTransform : DangerTransforms)
	{
		std::shared_ptr<CDamageBoxCollider> DangerBoxComp = CreateComponent<CDamageBoxCollider>("Danger").lock();
		DangerBoxComp->SetBoxSize(DangerTransform.first);
		DangerBoxComp->SetRelativePos(DangerTransform.second);
	}

	return true;
}

bool CStage1StaticTileMap0Object::InitDynamicObjects()
{
	return true;
}

#pragma endregion

// 1번째
#pragma region CStage1StaticTileMap1Object

CStage1StaticTileMap1Object::CStage1StaticTileMap1Object()
{
	SetClassType<CStage1StaticTileMap1Object>();
}

bool CStage1StaticTileMap1Object::Init()
{
	CStaticTileMapObject::Init();

	mPlayerRespawnOffsets[EMapMoveDirection::Down] = FVector3(104.f, 98.f, 0.f);
	mPlayerRespawnOffsets[EMapMoveDirection::Up] = FVector3(910.f, 228.f, 0.f);

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

void CStage1StaticTileMap1Object::LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData)
{
	CStaticTileMapObject::LoadMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage1StaticTileMap1Object::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData != nullptr)
	{
		if (CurRuntimeData->mGetStrawberry0 == true)
		{
			std::shared_ptr<CStrawberryObject> Strawberry = mStrawberry0.lock();
			if (Strawberry != nullptr || Strawberry->IsItemActivated() == false)
			{
				Strawberry->Destroy();
			}
		}
	}
}

void CStage1StaticTileMap1Object::SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const
{
	CStaticTileMapObject::SaveMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage1StaticTileMap1Object::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData != nullptr)
	{
		if (mStrawberry0.expired() == true || mStrawberry0.lock()->IsItemActivated() == true)
		{
			CurRuntimeData->mGetStrawberry0 = true;
		}
	}
}

bool CStage1StaticTileMap1Object::InitMainTileMap()
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
		LoadMap("Ice", TEXT("stage1_ice_2.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Cement");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile1");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Cement", TEXT("stage1_cement_2.tlm"));
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
		LoadMap("Snow", TEXT("stage1_snow_2.tlm"));
	}
	{
		FTileMapLayer& MapLayer = AddTileLayer("Girder");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile2");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Girder", TEXT("stage1_girder_2.tlm"));
	}

	return true;
}

bool CStage1StaticTileMap1Object::InitCollisions()
{
	static const std::vector<std::pair<FVector2, FVector2>> WallTransforms =
	{
		{{26.f, 624.f},			{13.f, 312.f}},
		{{260.f, 78.f},			{286.f, 39.f}},
		{{52.f, 130.f},			{390.f, 65.f}},
		{{52.f, 338.f},			{26.f, 455.f}},
		{{182.f, 182.f},		{143.f, 481.f}},
		{{26.f, 104.f},			{247.f, 546.f}},
		{{26.f, 130.f},			{377.f, 533.f}},
		{{858.f, 78.f},			{429.f, 611.f}},
		{{286.f, 52.f},			{559.f, 572.f}},
		{{286.f, 52.f},			{559.f, 572.f}},
		{{104.f, 130.f},		{572.f, 533.f}},
		{{78.f, 156.f},			{611.f, 520.f}},
		{{78.f, 234.f},			{585.f, 481.f}},
		{{156.f, 78.f},			{780.f, 559.f}},
		{{104.f, 104.f},		{806.f, 494.f}},
		{{26.f, 130.f},			{845.f, 403.f}},
		{{26.f, 364.f},			{871.f, 468.f}},

		{{52.f, 650.f},			{1014.f, 325.f}},
		{{156.f, 208.f},		{910.f, 104.f}},
		{{104.f, 130.f},		{780.f, 65.f}},
		{{130.f, 52.f},			{663.f, 26.f}},
		{{52.f, 78.f},			{650.f, 39.f}},
		{{78.f, 234.f},			{585.f, 117.f}},
		{{104.f, 78.f},			{572.f, 195.f}},
		{{104.f, 52.f},			{598.f, 208.f}},
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

bool CStage1StaticTileMap1Object::InitPlatforms()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> PlatformObjectSets =
	{
		{{26.f, 52.f},	{130.f, 26.f},	"T_Stage1_WoodPlatform_5x1"},
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

bool CStage1StaticTileMap1Object::InitEnvMeshes()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> EnvObjectSets =
	{
		{{221.f, 78.f},		{78.f, 104.f},		"T_Stage1_SignForwardPanel" },
		{{130.f, 78.f},		{104.f, 104.f},		"T_Stage1_Ladder" },
		{{78.f, 78.f},		{52.f, 52.f},		"T_Stage1_PaintBuckets" },
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

bool CStage1StaticTileMap1Object::InitBlockers()
{
	static const std::vector<std::pair<FVector2, FVector2>> BlockerTransforms =
	{
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

	static const std::vector<std::pair<FVector2, FVector2>> DangerTransforms =
	{
		{{780.f, 50.f},			{650.f, -40.f}},

		{{130.f, 20.f},			{585.f, 244.f}},
		{{52.f, 20.f},			{650.f, 88.f}},
		{{52.f, 20.f},			{702.f, 62.f}},
	};

	for (auto& DangerTransform : DangerTransforms)
	{
		std::shared_ptr<CDamageBoxCollider> DangerBoxComp = CreateComponent<CDamageBoxCollider>("Danger").lock();
		DangerBoxComp->SetBoxSize(DangerTransform.first);
		DangerBoxComp->SetRelativePos(DangerTransform.second);
	}

	return true;
}

bool CStage1StaticTileMap1Object::InitDynamicObjects()
{
	{
		FStrawberryInitInfo InitInfo;

		InitInfo.mWorldPos = FVector3(286.f, 468.f, 0.f);
		InitInfo.mMeshWorldPos = FVector3(286.f, 468.f, 0.f);
		InitInfo.mMeshScale = FVector3(58.5f, 52.f, 1.f);
		InitInfo.mRenderLayerName = "WorldWidget";
		InitInfo.mIsEnableAlpha = true;

		InitInfo.mCollisionSize = { 58.5f, 52.f };
		InitInfo.mStrawberryIndex = 0;

		mStrawberry0 = CStrawberryObject::MakeQuikly(mWorld.lock(), InitInfo);
		AddSubGameObject("Strawberry0", mStrawberry0);
	}

	{
		FSpringObjectInitInfo InitInfo;

		InitInfo.mWorldPos = FVector3(364.f, 130.f, 0.f);
		InitInfo.mMeshWorldPos = FVector3(364.f, 130.f, 0.f);
		InitInfo.mMeshScale = FVector3(52.f, 52.f, 1.f);
		InitInfo.mAnimationName = "A_WoodSpring";
		InitInfo.mRenderLayerName = "FrontObject";
		InitInfo.mIsEnableAlpha = true;

		InitInfo.mCollisionSize = { 52.f, 15.f };
		InitInfo.mSpringDir = ETouchedWall::Type::Up;
		InitInfo.mSpringVelocity = 200.f;

		AddSubGameObject("Spring", CSpringMapObject::MakeQuikly(mWorld.lock(), InitInfo));
	}

	return true;
}

#pragma endregion

// 2번째
#pragma region CStage1StaticTileMap2Object

CStage1StaticTileMap2Object::CStage1StaticTileMap2Object()
{
	SetClassType<CStage1StaticTileMap2Object>();
}

bool CStage1StaticTileMap2Object::Init()
{
	CStaticTileMapObject::Init();

	mPlayerRespawnOffsets[EMapMoveDirection::Down] = FVector3(104.f, 124.f, 0.f);
	//mPlayerRespawnOffsets[EMapMoveDirection::Up] = FVector3(819.f, 436.f, 0.f);

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

void CStage1StaticTileMap2Object::LoadMapRuntimeData(const std::shared_ptr<FMapRuntimeDataBase>& RuntimeData)
{
	CStaticTileMapObject::LoadMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage1StaticTileMap2Object::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData != nullptr)
	{
		if (CurRuntimeData->mGetStrawberry1 == true)
		{
			std::shared_ptr<CStrawberryObject> Strawberry = mStrawberry1.lock();
			if (Strawberry != nullptr || Strawberry->IsItemActivated() == false)
			{
				Strawberry->Destroy();
			}
		}
	}
}

void CStage1StaticTileMap2Object::SaveMapRuntimeData(std::shared_ptr<FMapRuntimeDataBase>& RuntimeData) const
{
	CStaticTileMapObject::SaveMapRuntimeData(RuntimeData);

	auto CurRuntimeData = std::static_pointer_cast<CStage1StaticTileMap2Object::FMapRuntimeData>(RuntimeData);
	if (CurRuntimeData != nullptr)
	{
		if (mStrawberry1.expired() == true || mStrawberry1.lock()->IsItemActivated() == true)
		{
			CurRuntimeData->mGetStrawberry1 = true;
		}
	}
}

bool CStage1StaticTileMap2Object::InitMainTileMap()
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
		LoadMap("Ice", TEXT("stage1_ice_3.tlm"));
	}

	{
		FTileMapLayer& MapLayer = AddTileLayer("Cement");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile1");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Cement", TEXT("stage1_cement_3.tlm"));
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
		LoadMap("Snow", TEXT("stage1_snow_3.tlm"));
	}
	{
		FTileMapLayer& MapLayer = AddTileLayer("Girder");
		std::shared_ptr<CTileMapRender> TileRenderComp = MapLayer.mTileRenderComponent.lock();
		if (TileRenderComp == nullptr)
		{
			return false;
		}
		TileRenderComp->SetRenderLayer("FrontTile2");
		TileRenderComp->SetRelativePos(0.f, 0.f);
		LoadMap("Girder", TEXT("stage1_girder_3.tlm"));
	}

	return true;
}

bool CStage1StaticTileMap2Object::InitCollisions()
{
	static const std::vector<std::pair<FVector2, FVector2>> WallTransforms =
	{
		{{78.f, 104.f},			{39.f, 52.f}},
		{{52.f, 390.f},			{26.f, 273.f}},
		{{78.f, 156.f},			{39.f, 520.f}},
		{{104.f, 104.f},		{130.f, 546.f}},
		{{884.f, 104.f},		{442.f, 598.f}},
		{{52.f, 104.f},			{442.f, 546.f}},
		{{234.f, 78.f},			{585.f, 559.f}},
		{{26.f, 130.f},			{715.f, 533.f}},
		{{52.f, 130.f},			{858.f, 585.f}},
		{{26.f, 156.f},			{871.f, 572.f}},

		{{52.f, 52.f},			{286.f, 364.f}},
		{{26.f, 130.f},			{325.f, 325.f}},

		{{26.f, 130.f},			{455.f, 221.f}},
		{{52.f, 78.f},			{494.f, 195.f}},
		{{26.f, 130.f},			{507.f, 169.f}},
		{{26.f, 156.f},			{533.f, 156.f}},

		{{52.f, 78.f},			{858.f, 351.f}},
		{{26.f, 182.f},			{845.f, 299.f}},
		{{26.f, 156.f},			{819.f, 312.f}},

		{{78.f, 52.f},			{221.f, 26.f}},

		{{442.f, 78.f},			{845.f, 39.f}},
		{{78.f, 156.f},			{741.f, 78.f}},
		{{130.f, 130.f},		{845.f, 65.f}},
		{{104.f, 78.f},			{962.f, 39.f}},
		{{52.f, 104.f},			{1040.f, 52.f}},
		{{26.f, 650.f},			{1053.f, 325.f}},
		{{26.f, 364.f},			{1027.f, 468.f}},
		{{26.f, 234.f},			{1001.f, 533.f}},
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

bool CStage1StaticTileMap2Object::InitPlatforms()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> PlatformObjectSets =
	{
		{{78.f, 26.f},	{104.f, 26.f},	"T_Stage1_WoodPlatform_4x1"},
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

bool CStage1StaticTileMap2Object::InitEnvMeshes()
{
	static const std::vector<std::tuple<FVector2, FVector2, std::string>> EnvObjectSets =
	{
		{{169.f, 416.f},		{39.f, 78.f},		"T_Stage1_TraficLightOff" },
		{{520.f, 234.f},		{58.5f, 152.75f},	"T_Stage1_SignPark" },
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

bool CStage1StaticTileMap2Object::InitBlockers()
{
	static const std::vector<std::pair<FVector2, FVector2>> BlockerTransforms =
	{
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

	static const std::vector<std::pair<FVector2, FVector2>> DangerTransforms =
	{
		{{832.f, 50.f},			{650.f, -40.f}},

		{{78.f, 20.f},			{663.f, 88.f}},
		{{78.f, 20.f},			{741.f, 166.f}},
		{{130.f, 20.f},			{845.f, 140.f}},
		{{104.f, 20.f},			{962.f, 88.f}},
		{{26.f, 20.f},			{1027.f, 114.f}},
	};

	for (auto& DangerTransform : DangerTransforms)
	{
		std::shared_ptr<CDamageBoxCollider> DangerBoxComp = CreateComponent<CDamageBoxCollider>("Danger").lock();
		DangerBoxComp->SetBoxSize(DangerTransform.first);
		DangerBoxComp->SetRelativePos(DangerTransform.second);
	}

	return true;
}

bool CStage1StaticTileMap2Object::InitDynamicObjects()
{
	{
		FStrawberryInitInfo InitInfo;

		InitInfo.mWorldPos = FVector3(936.f, 156.f, 0.f);
		InitInfo.mMeshWorldPos = FVector3(936.f, 156.f, 0.f);
		InitInfo.mMeshScale = FVector3(58.5f, 52.f, 1.f);
		InitInfo.mRenderLayerName = "WorldWidget";
		InitInfo.mIsEnableAlpha = true;

		InitInfo.mCollisionSize = { 58.5f, 52.f };
		InitInfo.mStrawberryIndex = 1;

		mStrawberry1 = CStrawberryObject::MakeQuikly(mWorld.lock(), InitInfo);
		AddSubGameObject("Strawberry0", mStrawberry1);
	}

	return true;
}

#pragma endregion