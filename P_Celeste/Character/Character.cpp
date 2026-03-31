#include "Pch.h"
#include "Character.h"

#include "World/World.h"

#include "Component/ColliderBox2D.h"

#include "Component/MeshComponent.h"
#include "Component/CharacterMovementComponent.h"
#include "Component/Animation2DComponent.h"

#include "Animation/AnimStateMachine.h"

CCharacter::CCharacter()
{
}

CCharacter::~CCharacter()
{
}

bool CCharacter::Init()
{
	CGameObject::Init();

	mBodyComponent = MakeRootCollider();

	mMeshComponent = CreateComponent<CMeshComponent>("MainMesh").lock();
	if (mMeshComponent != nullptr)
	{
		mMeshComponent->SetShader("DefaultTexture2D");
		mMeshComponent->SetMesh("RectTex");
		mMeshComponent->SetWorldScale(100.f, 100.f);
		mMeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		mMeshComponent->SetBlendState(0, "AlphaBlend");
		mMeshComponent->SetPivot(0.5f, 0.5f);
	}
	mAnimation2DComponent = CreateComponent<CAnimation2DComponent>("Animation2D").lock();
	if (mAnimation2DComponent != nullptr)
	{
		mAnimation2DComponent->SetUpdateComponent(mMeshComponent);
	}
	mMovementComponent = CreateComponent<CCharacterMovementComponent>("Movement").lock();
	if (mMovementComponent != nullptr)
	{
		mMovementComponent->SetUpdateComponent(GetRootComponent());
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	if (World != nullptr)
	{
		std::shared_ptr<CInput> InputManager = World->GetInput().lock();
		if (InputManager != nullptr)
		{
			BindPlayerInputs(InputManager);
		}
	}

	mAnimStateMachine = std::make_shared<CAnimStateMachine>();
	if (mAnimStateMachine != nullptr)
	{
		mAnimStateMachine->SetTargetComponent(mAnimation2DComponent);
	}
	ConfigStateMachine(mAnimStateMachine);

	return true;
}

void CCharacter::Update(float DeltaTime)
{
	CGameObject::Update(DeltaTime);
}

void CCharacter::PostUpdate(float DeltaTime)
{
	CGameObject::PostUpdate(DeltaTime);

	mAnimStateMachine->Update(DeltaTime, mMovementComponent->GetHorizonSight());
}

const FVector3& CCharacter::GetSightDir()
{
	if (mMovementComponent == nullptr)
	{
		return FVector3::Zero;
	}
	return mMovementComponent->GetSight();
}

void CCharacter::SetSightDir(const FVector3& SightDir)
{
	if (mMovementComponent == nullptr)
	{
		return;
	}
	mMovementComponent->SetSight(SightDir);
}

std::vector<std::string> CCharacter::GetEmotionTextureNames(const std::string& EmotionName, int32 Count, int32 StartOffset)
{
	std::vector<std::string> Textures;
	for (int i = 0; i < Count; ++i)
	{
		Textures.push_back(EmotionName + std::to_string(i + StartOffset));
	}
	return Textures;
}

std::vector<std::pair<std::string, int32>> CCharacter::GetEmotionSoundNames(const std::string& EmotionName, int32 SampleCount, int32 Count, int32 StartOffset)
{
	std::vector<std::pair<std::string, int32>> Textures;
	for (int i = 0; i < Count; ++i)
	{
		Textures.push_back({ EmotionName + std::to_string(i + StartOffset) + '_', SampleCount});
	}
	return Textures;
}




