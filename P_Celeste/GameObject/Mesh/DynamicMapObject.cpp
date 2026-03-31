#include "Pch.h"
#include "DynamicMapObject.h"

#include "World/WorldAssetManager.h"

#include "Component/Collider.h"
#include "Component/MeshComponent.h"

int32 CDynamicMapObject::mMapMaxIndex = 0;

bool CDynamicMapObject::Init()
{
	CPlayerInteractableObject::Init();

	std::shared_ptr<CSceneComponent> RootComp = CreateComponent<CSceneComponent>("RootComp").lock();
	RootComp->SetWorldPos(0.f, 0.f);
	RootComp->SetWorldScale(1.f, 1.f);

	mBodyComponent = MakeMainCollider();
	if (mBodyComponent != nullptr)
	{
		mBodyComponent->SetStatic(false);
		mBodyComponent->SetInvMass(0.f);
	}

	mMeshComponent = CreateComponent<CMeshComponent>("MeshComp").lock();
	if (mMeshComponent != nullptr)
	{
		mMeshComponent->SetShader("DefaultTexture2D");
		mMeshComponent->SetMesh("RectTex");
		mMeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		mMeshComponent->SetRelativePos(0.f, 0.f);
		mMeshComponent->SetRelativeScale(1.f, 1.f);
		mMeshComponent->SetPivot(0.f, 0.f);
	}

	return true;
}

void CDynamicMapObject::LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData)
{
}

void CDynamicMapObject::SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const
{
}

void CDynamicMapObject::OnLanded(std::shared_ptr<CGameObject> Interator)
{
	AttachGameObject(Interator);
}

void CDynamicMapObject::OnJumped(std::shared_ptr<CGameObject> Interator)
{
	DetachGameObject(Interator);
}

void CDynamicMapObject::OnGraped(std::shared_ptr<CGameObject> Interator)
{
	AttachGameObject(Interator);
}

void CDynamicMapObject::OnReleased(std::shared_ptr<CGameObject> Interator)
{
	DetachGameObject(Interator);
}

void CDynamicMapObject::MakeQuikly(std::shared_ptr<CDynamicMapObject> MapObject, const FDynamicObjectInitInfo& InitInfo)
{
	std::shared_ptr<CWorld> World = MapObject->mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	MapObject->SetWorldPos(InitInfo.mWorldPos);

	if (MapObject->mMeshComponent != nullptr)
	{
		if (InitInfo.mIsEnableAlpha == true)
		{
			MapObject->mMeshComponent->SetBlendState(0, "AlphaBlend");
		}
		MapObject->mMeshComponent->SetWorldPos(InitInfo.mMeshWorldPos);
		MapObject->mMeshComponent->SetRelativeScale(InitInfo.mMeshScale);
		MapObject->mMeshComponent->SetRenderLayer(InitInfo.mRenderLayerName);
		if (InitInfo.mTextureName.empty() == false)
		{
			MapObject->mMeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture(InitInfo.mTextureName));
		}
	}
}