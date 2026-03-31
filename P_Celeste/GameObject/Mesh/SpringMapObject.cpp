#include "Pch.h"
#include "SpringMapObject.h"

#include "Character/Player/Player.h"

#include "Component/ColliderBox2D.h"
#include "Component/MeshComponent.h"
#include "Component/Animation2DComponent.h"
#include "Component/CharacterMovementComponent.h"

bool CSpringMapObject::Init()
{
	CDynamicMapObject::Init();

	mAnimation2DComponent = CreateComponent<CAnimation2DComponent>("Animation2D").lock();
	if (mAnimation2DComponent != nullptr)
	{
		mAnimation2DComponent->SetUpdateComponent(mMeshComponent);
	}
	return true;
}

void CSpringMapObject::Update(float DeltaTime)
{
	CDynamicMapObject::Update(DeltaTime);

	if (mSpringState != ESpringState::Idle)
	{
		if (mSpringState == ESpringState::Trigger)
		{
			mSpringState = ESpringState::Reload;

			std::shared_ptr<CCharacter> TriggeredInterator = mTriggeredInterator.lock();
			TriggeredInterator->GetMovementComp()->StopAllMovement();
			TriggeredInterator->GetMovementComp()->DoMovement("Jump");
			TriggeredInterator->GetMovementComp()->GetUpdateComponent().lock()->AddPhysicsVelocity(mMeshComponent->GetAxis(EAxis::Y) * mSpringVelocity);
		
			mWorld.lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_Spring", GetWorldPos());
		}

		mTriggerDelayAccTime += DeltaTime;
	}
}

std::shared_ptr<CCollider> CSpringMapObject::MakeMainCollider()
{
	std::shared_ptr<CColliderBox2D> SpringBoxComp = CreateComponent<CColliderBox2D>("SpringCollider").lock();

	SpringBoxComp->SetCollisionProfile("Dynamic");
#ifdef _DEBUG

	SpringBoxComp->SetDebugDraw(true);

#endif // DEBUG
	SpringBoxComp->SetInheritScale(false);

	return SpringBoxComp;
}

void CSpringMapObject::OnLanded(std::shared_ptr<CGameObject> Interator)
{
	if (CanTrigger(ETouchedWall::Type::Down) == false)
	{
		return;
	}

	auto Player = std::static_pointer_cast<CPlayer>(Interator);
	Player->ForcedRechageMovements();

	TriggerSpring(Player);
}

void CSpringMapObject::OnJumped(std::shared_ptr<CGameObject> Interator)
{
}

void CSpringMapObject::OnGraped(std::shared_ptr<CGameObject> Interator)
{
}

void CSpringMapObject::OnReleased(std::shared_ptr<CGameObject> Interator)
{
}

void CSpringMapObject::OnTouchedDown(ETouchedWall::Type Type, std::shared_ptr<CGameObject> Interator)
{
	CDynamicMapObject::OnTouchedDown(Type, Interator);

	if (Type == ETouchedWall::Type::Down)
	{
		return;
	}

	if (CanTrigger(Type) == false)
	{
		return;
	}

	auto Player = std::static_pointer_cast<CPlayer>(Interator);
	Player->ForcedRechageMovements();

	TriggerSpring(Player);
}

void CSpringMapObject::TriggerSpring(std::shared_ptr<CCharacter> Interator)
{
	mSpringState = ESpringState::Trigger;
	mTriggerDelayAccTime = 0.f;

	mTriggeredInterator = Interator;

	mAnimation2DComponent->RestartAnimation();
	mAnimation2DComponent->SetFinishNotify(mAnimation2DComponent->GetAnimationSplitName(), [this]() {
		mSpringState = ESpringState::Idle;
		});
}

bool CSpringMapObject::CanTrigger(ETouchedWall::Type Type) const
{
	if (mTriggerDelayAccTime < mTriggerDelayTime)
	{
		return false;
	}

	switch (Type)
	{
	case ETouchedWall::Type::Up:
		return mSpringDir == ETouchedWall::Type::Down;
	case ETouchedWall::Type::Down:
		return mSpringDir == ETouchedWall::Type::Up;
	case ETouchedWall::Type::Left:
		return mSpringDir == ETouchedWall::Type::Right;
	case ETouchedWall::Type::Right:
		return mSpringDir == ETouchedWall::Type::Left;
	}

	return false;
}

std::shared_ptr<CSpringMapObject> CSpringMapObject::MakeQuikly(std::shared_ptr<CWorld> World, const FSpringObjectInitInfo& InitInfo)
{
	std::shared_ptr<CSpringMapObject> SpringMap = CDynamicMapObject::MakeQuikly<CSpringMapObject>(World, InitInfo);
	MakeQuikly(SpringMap, InitInfo);

	return SpringMap;
}

void CSpringMapObject::MakeQuikly(std::shared_ptr<CSpringMapObject> SpringMap, const FSpringObjectInitInfo& InitInfo)
{
	CDynamicMapObject::MakeQuikly(SpringMap, InitInfo);

	std::shared_ptr<CColliderBox2D> BoxCollisionComp = std::static_pointer_cast<CColliderBox2D>(SpringMap->mBodyComponent);
	if (BoxCollisionComp != nullptr)
	{
		BoxCollisionComp->SetRelativePos(InitInfo.mCollisionSize / 2.f);
		BoxCollisionComp->SetBoxSize(InitInfo.mCollisionSize);
	}

	std::shared_ptr<CMeshComponent>& MeshComp = SpringMap->mMeshComponent;
	if (MeshComp != nullptr)
	{
		switch (InitInfo.mSpringDir)
		{
		case ETouchedWall::Type::Up:
			MeshComp->SetWorldRotationZ(0.f);
			break;
		case ETouchedWall::Type::Down:
			MeshComp->SetWorldRotationZ(180.f);
			break;
		case ETouchedWall::Type::Left:
			MeshComp->SetWorldRotationZ(90.f);
			break;
		case ETouchedWall::Type::Right:
			MeshComp->SetWorldRotationZ(270.f);
			break;
		}

		SpringMap->mSpringDir = InitInfo.mSpringDir;
		SpringMap->mSpringVelocity = InitInfo.mSpringVelocity;
	}

	std::shared_ptr<CAnimation2DComponent>& AnimComp = SpringMap->mAnimation2DComponent;
	if (AnimComp != nullptr)
	{
		AnimComp->AddAnimation(InitInfo.mAnimationName);
	}
}
