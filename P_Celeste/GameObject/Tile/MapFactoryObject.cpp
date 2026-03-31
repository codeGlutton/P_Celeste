#include "Pch.h"
#include "MapFactoryObject.h"

#include "GameObject/Tile/StaticTileMapObject.h"

#include "World/World.h"
#include "World/CameraManager.h"

#include "Component/MeshComponent.h"

#include "Character/Player/Player.h"

bool CMapFactoryObject::Init()
{
	CGameObject::Init();

	std::shared_ptr<CSceneComponent> RootComp = CreateComponent<CSceneComponent>("RootComp").lock();
	if (RootComp != nullptr)
	{
		RootComp->SetPivot(0.f, 0.f);
		RootComp->SetWorldPos(0.f, 0.f);
	}

	if (InitMapInfos() == false)
	{
		return false;
	}
	if (InitBackgrounds() == false)
	{
		return false;
	}
	if (InitDynamicObjects() == false)
	{
		return false;
	}
	if (InitUI() == false)
	{
		return false;
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<FMapInfoBase> StartMapInfo = mCurMapInfo.lock();
	if (StartMapInfo == nullptr)
	{
		return false;
	}

	StartMapInfo->StartMap(World, GetWorldPos());
	return true;
}

void CMapFactoryObject::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);

	std::shared_ptr<FMapInfoBase> CurMapInfo = mCurMapInfo.lock();
	std::shared_ptr<CGameObject> TargetObject = mTargetObject.lock();
	if (CurMapInfo == nullptr || TargetObject == nullptr)
	{
		return;
	}

	FVector2 TargetWorldPos = { TargetObject->GetWorldPos().x, TargetObject->GetWorldPos().y };
	EMapMoveDirection::Type OutDir;
	if (CurMapInfo->IsInCurTileMap(TargetWorldPos, OUT OutDir) == false)
	{
		std::shared_ptr<CWorld> World = mWorld.lock();
		mCurMapInfo = CurMapInfo->MoveMap(World, OutDir);
		mLastMoveDir = EMapMoveDirection::ReverseType(OutDir);

		for (auto& Event : mOnChangeMap)
		{
			Event();
		}
	}
}

void CMapFactoryObject::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CCameraManager> CameraMgr = World->GetCameraManager().lock();

	for (auto& BackgroundComp : mBackgroundComps)
	{
		std::shared_ptr<CSceneComponent> Comp = BackgroundComp.mSceneComp.lock();
		if (Comp == nullptr)
		{
			continue;
		}
		
		FVector3 TargetPos = (CameraMgr->GetMainCameraWorldPos() - mParallZeroPos) * BackgroundComp.mFactor + BackgroundComp.mOriginWorldPos;
		FVector3 BackgroundPos = MathUtils::Lerp(Comp->GetWorldPos(), TargetPos, mParallaxSmoothRate);
		Comp->SetWorldPos(BackgroundPos);
	}
}

void CMapFactoryObject::StartOpening()
{
	std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(mTargetObject.lock());
	Player->PlayRespawnCutscene();
}

void CMapFactoryObject::StartEnding()
{
	std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(mTargetObject.lock());
	Player->PlayDeadCutscene();
}

void CMapFactoryObject::SetTargetObject(std::shared_ptr<CGameObject> Target)
{
	if (Target == nullptr)
	{
		return;
	}

	mTargetObject = Target;
	RespawnTargetObject();
}

void CMapFactoryObject::RespawnTargetObject() const
{
	std::shared_ptr<FMapInfoBase> CurMapInfo = mCurMapInfo.lock();
	std::shared_ptr<CGameObject> TargetObject = mTargetObject.lock();
	if (CurMapInfo == nullptr || TargetObject == nullptr)
	{
		return;
	}
	CurMapInfo->RespawnTargetObject(mLastMoveDir, mTargetObject);
}

bool CMapFactoryObject::InitSimpleBackground(int32 IndexX, int32 IndexY, const FVector3& BaseWorldPos, const FVector3& WorldScale, const std::string& RenderLayer, const std::string& TextureName, const FVector2& ParallaxScrollRate)
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	FVector2 MeshBackgroundPos = { BaseWorldPos.x + WorldScale.x * IndexX, BaseWorldPos.y + WorldScale.y * IndexY };
	std::shared_ptr<CMeshComponent> MeshComponent = CreateComponent<CMeshComponent>("Background").lock();
	MeshComponent->SetShader("DefaultTexture2D");
	MeshComponent->SetMesh("RectTex");
	MeshComponent->SetWorldPos(MeshBackgroundPos);
	MeshComponent->SetWorldScale(WorldScale);
	MeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
	MeshComponent->SetBlendState(0, "AlphaBlend");
	MeshComponent->SetRenderLayer(RenderLayer);
	MeshComponent->SetPivot(0.f, 0.f);
	MeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture(TextureName));

	mBackgroundComps.push_back({ ParallaxScrollRate, MeshBackgroundPos, MeshComponent });

	return true;
}

bool CMapFactoryObject::InitSimpleBackgrounds(int32 StartX, int32 StartY, int32 CountX, int32 CountY, const FVector3& BaseWorldPos, const FVector3& WorldScale, const std::string& RenderLayer, const std::string& TextureName, const FVector2& ParallaxScrollRate)
{
	for (int32 x = 0; x < CountX; ++x)
	{
		for (int32 y = 0; y < CountY; ++y)
		{
			InitSimpleBackground(StartX + x, StartY + y, BaseWorldPos, WorldScale, RenderLayer, TextureName, ParallaxScrollRate);
		}
	}

	return true;
}

void CMapFactoryObject::LoadSerialTextures(const std::string& TextureName, const std::wstring& FilePathFormat, int32 Count, int32 StartOffset)
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (int i = 0; i < Count; ++i)
	{
		TCHAR* FileName = new TCHAR[MAX_PATH];
		memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
		wsprintf(FileName, FilePathFormat.c_str(), i + StartOffset);

		WorldAssetMgr->LoadTexture(TextureName + std::to_string(i), FileName);

		delete[] FileName;
	}
}

void CMapFactoryObject::LoadSerialAnimation2D(const int32 FileOffset, const int32 FileCount, const std::string& AnimName, const TCHAR* AnimFileNameFormat, const FVector2& FrameSize)
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	std::string AnimAssetName = "A_" + AnimName;
	std::string TextureAssetName = "T_" + AnimName;

	WorldAssetMgr->CreateAnimation(AnimAssetName);
	WorldAssetMgr->SetAnimation2DTextureType(AnimAssetName, EAnimation2DTextureType::Frame);

	std::vector<const TCHAR*> TextureFileNames;
	for (int i = 0; i < FileCount; ++i)
	{
		TCHAR* FileName = new TCHAR[MAX_PATH];
		memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
		wsprintf(FileName, AnimFileNameFormat, i + FileOffset);
		TextureFileNames.push_back(FileName);
	}

	WorldAssetMgr->SetTexture(AnimAssetName, TextureAssetName, TextureFileNames);

	for (int i = 0; i < FileCount; ++i)
	{
		delete[] TextureFileNames[i];
	}
	TextureFileNames.clear();

	WorldAssetMgr->AddFrame(AnimAssetName, FileCount, 0.f, 0.f, FrameSize.x, FrameSize.y);
}

void CMapFactoryObject::LoadSerialSounds(const std::string& TextureName, const std::string& GroupName, bool IsLoop, const std::string& FilePathFormat, int32 Count, int32 StartOffset)
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (int i = 0; i < Count; ++i)
	{
		char* FileName = new char[MAX_PATH];
		memset(FileName, 0, MAX_PATH);
		sprintf_s(FileName, MAX_PATH, FilePathFormat.c_str(), i + StartOffset);

		WorldAssetMgr->LoadSound(TextureName + std::to_string(i), GroupName, IsLoop, FileName);

		delete[] FileName;
	}
}

void CMapFactoryObject::LoadSerialSound3Ds(const std::string& TextureName, const std::string& GroupName, bool IsLoop, const std::string& FilePathFormat, int32 Count, int32 StartOffset)
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	for (int i = 0; i < Count; ++i)
	{
		char* FileName = new char[MAX_PATH];
		memset(FileName, 0, MAX_PATH);
		sprintf_s(FileName, MAX_PATH, FilePathFormat.c_str(), i + StartOffset);

		WorldAssetMgr->LoadSound3D(TextureName + std::to_string(i), GroupName, IsLoop, FileName);

		delete[] FileName;
	}
}
