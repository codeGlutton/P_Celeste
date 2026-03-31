#include "Pch.h"
#include "ItemObject.h"

#include "Component/MeshComponent.h"
#include "Component/Collider.h"
#include "Component/SpringArmComponent.h"

#include "World/WorldAssetManager.h"

int32 CItemObject::mItemMaxIndex = 0;

bool CItemObject::Init()
{
    CGameObject::Init();

	std::shared_ptr<CSceneComponent> RootComp = CreateComponent<CSceneComponent>("RootComp").lock();
	RootComp->SetWorldPos(0.f, 0.f);
	RootComp->SetWorldScale(1.f, 1.f);

	mSpringArmComponent = CreateComponent<CSpringArmComponent>("SpringArm").lock();
	if (mSpringArmComponent != nullptr)
	{
		mSpringArmComponent->SetMaxDist(5.f);
	}

    mBodyComponent = MakeMainCollider();
	if (mBodyComponent != nullptr)
	{
		mBodyComponent->SetStatic(false);
		mBodyComponent->SetCollisionProfile("Item");
		mBodyComponent->SetCollisionBeginFunction<CItemObject>(this, &CItemObject::OnBeginCollider);
	}

	mMeshComponent = CreateComponent<CMeshComponent>("MeshComp", "SpringArm").lock();
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

void CItemObject::OnActiveItem(std::shared_ptr<CPlayer> Player)
{
	mIsItemActivated = true;
}

void CItemObject::MakeQuikly(std::shared_ptr<CItemObject> ItemObj, const FItemObjectInitInfo& InitInfo)
{
	std::shared_ptr<CWorld> World = ItemObj->mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	ItemObj->SetWorldPos(InitInfo.mWorldPos);

	if (ItemObj->mMeshComponent != nullptr)
	{
		if (InitInfo.mIsEnableAlpha == true)
		{
			ItemObj->mMeshComponent->SetBlendState(0, "AlphaBlend");
		}
		ItemObj->mMeshComponent->SetWorldPos(InitInfo.mMeshWorldPos);
		ItemObj->mMeshComponent->SetRelativeScale(InitInfo.mMeshScale);
		ItemObj->mMeshComponent->SetRenderLayer(InitInfo.mRenderLayerName);
		if (InitInfo.mTextureName.empty() == false)
		{
			ItemObj->mMeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture(InitInfo.mTextureName));
		}
	}
}
