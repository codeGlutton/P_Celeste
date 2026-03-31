#include "Pch.h"
#include "HairEditorObject.h"

#include "Component/MeshComponent.h"
#include "Component/Animation2DComponent.h"

#include "World/World.h"
#include "World/WorldAssetManager.h"

CHairEditorObject::CHairEditorObject()
{
	SetClassType<CHairEditorObject>();
}

bool CHairEditorObject::Init()
{
	CGameObject::Init();

	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldAssetManager> WorldAssetMgr = World->GetWorldAssetManager().lock();

	mCharacterMeshComponent = CreateComponent<CMeshComponent>("CharacterMesh").lock();
	if (mCharacterMeshComponent != nullptr)
	{
		mCharacterMeshComponent->SetShader("DefaultTexture2D");
		mCharacterMeshComponent->SetMesh("RectTex");
		mCharacterMeshComponent->SetWorldScale(100.f, 100.f);
		mCharacterMeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		mCharacterMeshComponent->SetBlendState(0, "AlphaBlend");
		mCharacterMeshComponent->SetPivot(0.5f, 0.5f);
	}

	const float HairTexScale = 100.f * 10.f / 32.f;
	mBackHairMeshComponent = CreateComponent<CMeshComponent>("BackHairMesh").lock();
	if (mBackHairMeshComponent != nullptr)
	{
		mBackHairMeshComponent->SetShader("DefaultTexture2D");
		mBackHairMeshComponent->SetMesh("RectTex");
		mBackHairMeshComponent->SetWorldScale(HairTexScale, HairTexScale);
		mBackHairMeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		mBackHairMeshComponent->SetBlendState(0, "AlphaBlend");
		mBackHairMeshComponent->SetPivot(0.5f, 0.5f);

		mBackHairMeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture("T_PlayerBackHair"));

		mBackHairMeshComponent->SetRenderLayer("PlayerBackSubObject");
	}

	mFrontHairMeshComponent = CreateComponent<CMeshComponent>("FrontHairMesh").lock();
	if (mFrontHairMeshComponent != nullptr)
	{
		mFrontHairMeshComponent->SetShader("DefaultTexture2D");
		mFrontHairMeshComponent->SetMesh("RectTex");
		mFrontHairMeshComponent->SetWorldScale(HairTexScale, HairTexScale);
		mFrontHairMeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		mFrontHairMeshComponent->SetBlendState(0, "AlphaBlend");
		mFrontHairMeshComponent->SetPivot(0.5f, 0.5f);

		mFrontHairMeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture("T_PlayerFrontHair"));

		mFrontHairMeshComponent->SetRenderLayer("PlayerBackSubObject");
	}

	mFrontHairAnimComponent = CreateComponent<CAnimation2DComponent>("FrontHairAnim").lock();
	if (mFrontHairAnimComponent != nullptr)
	{
		mFrontHairAnimComponent->SetUpdateComponent(mFrontHairMeshComponent);
		mFrontHairAnimComponent->AddAnimation("A_Empty");
	}

	if (mFrontHairMeshComponent != nullptr)
	{
		mFrontHairMeshComponent->SetTexture(0, 0, WorldAssetMgr->FindTexture("T_PlayerFrontHair"));
	}

	return true;
}

void CHairEditorObject::SetTexture(const std::shared_ptr<CTexture>& Texture, int32 FrameIndex)
{
	if (mCharacterMeshComponent != nullptr)
	{
		mCharacterMeshComponent->SetTexture(0, FrameIndex, Texture);
	}
}

void CHairEditorObject::SetOffset(bool IsFrontHair, const FVector2& Offset)
{
	if (IsFrontHair == true)
	{
		mFrontHairMeshComponent->SetWorldPos(mCharacterMeshComponent->GetWorldPos() + Offset);
	}
	else
	{
		mBackHairMeshComponent->SetWorldPos(mCharacterMeshComponent->GetWorldPos() + Offset);
	}
}

void CHairEditorObject::SetFlip(bool IsFlip)
{
	mFrontHairAnimComponent->SetSymmetry("A_Empty", IsFlip);
}
