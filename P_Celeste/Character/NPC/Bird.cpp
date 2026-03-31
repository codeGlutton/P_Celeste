#include "Pch.h"
#include "Bird.h"

#include "Component/MeshComponent.h"
#include "Component/Animation2DComponent.h"
#include "Component/ColliderBox2D.h"
#include "Component/WidgetComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "World/TileMainWorld.h"

#include "Movement/SharedGameState.h"
#include "Movement/NPC/AIFlyMovement.h"

#include "UI/Main/SimpleTalkWidget.h"

CBird::CBird()
{
	SetClassType<CBird>();
}

bool CBird::Init()
{
	CCharacter::Init();

	InitCharacterMovements();

	mWidgetComponent = CreateComponent<CWidgetComponent>("DialogueWidget").lock();
	if (mWidgetComponent != nullptr)
	{
		std::shared_ptr<CSimpleTalkWidget> TalkWidget = mWidgetComponent->SetWidget<CSimpleTalkWidget>("Dialogue").lock();
		const float BoxSizeY = TalkWidget->GetMaxTextBoxSize().y * 0.5f + TalkWidget->GetMaxTextPointSize().y;
		const float TotalOffsetY = std::static_pointer_cast<CColliderBox2D>(mBodyComponent)->GetInfo().HalfSize.y + BoxSizeY + mTalkWidgetOffsetY;

		mWidgetComponent->SetRelativePos(0.f, TotalOffsetY);
		mWidgetComponent->SetRenderLayer("WorldWidget");
	}

	if (mMovementComponent != nullptr)
	{
		mMovementComponent->SetSpeed(320.f);
		mMovementComponent->SetAccel(500.f);
	}

	if (mMeshComponent != nullptr)
	{
		mMeshComponent->SetRelativePos(0.f, 20.f);
		mMeshComponent->SetRelativeScale(100.f, 75.f);
		mMeshComponent->SetRenderLayer("BackObject");
	}

	return true;
}

std::shared_ptr<CCollider> CBird::MakeRootCollider()
{
	std::shared_ptr<CColliderBox2D> Body = CreateComponent<CColliderBox2D>("Body").lock();
	if (Body != nullptr)
	{
		Body->SetCollisionProfile("NPC");
		Body->SetBoxSize(40.f, 40.f);

#ifdef _DEBUG

		Body->SetDebugDraw(true);

#endif // _DEBUG

		Body->SetInheritScale(true);
		Body->SetRelativePos(0.f, 0.f);
	}
	return Body;
}

void CBird::ConfigStateMachine(std::shared_ptr<CAnimStateMachine>& StateMachine)
{
	if (mAnimation2DComponent != nullptr)
	{
		mAnimation2DComponent->AddAnimation("A_BirdCry");
		mAnimation2DComponent->AddNotify("A_BirdCry", "E_BirdCry", 3, [World = mWorld, this]() {
			World.lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_BirdCry", GetWorldPos());
			});
		mAnimation2DComponent->AddAnimation("A_BirdBackFly");
		mAnimation2DComponent->AddNotify("A_BirdBackFly", "E_BirdBackFly0", 3, [World = mWorld, this]() {
			int32 Idx = std::rand() % 6;
			World.lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_BirdWingFlap" + std::to_string(Idx), GetWorldPos());
			});
		mAnimation2DComponent->AddAnimation("A_BirdIdle");
		mAnimation2DComponent->AddAnimation("A_BirdPeck");
		mAnimation2DComponent->AddNotify("A_BirdPeck", "E_BirdPeck0", 4, [World = mWorld, this]() {
			int32 Idx = std::rand() % 4;
			World.lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_BirdPeckA" + std::to_string(Idx), GetWorldPos());
			});
		mAnimation2DComponent->AddNotify("A_BirdPeck", "E_BirdPeck1", 7, [World = mWorld, this]() {
			int32 Idx = std::rand() % 4;
			World.lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_BirdPeckB" + std::to_string(Idx), GetWorldPos());
			});
		mAnimation2DComponent->AddAnimation("A_BirdFly");
	}

	{
		StateMachine->CreateState("Idle", [this]() {
			return ApplyBirdAnim("A_BirdIdle", 1.f, 1.f, true);
			}, true);

		StateMachine->CreateState("Peck", [this]() {
			return ApplyBirdAnim("A_BirdPeck", 1.f, 1.f, false);
			});
		StateMachine->CreateState("Cry", [this]() {
			return ApplyBirdAnim("A_BirdCry", 1.f, 1.f, false);
			});

		StateMachine->CreateState("FlyStart", [this]() {
			return ApplyBirdAnim("A_BirdBackFly", 1.f, 1.f, false);
			});
		StateMachine->CreateState("Fly", [this]() {
			return ApplyBirdAnim("A_BirdFly", 1.f, 1.f, true);
			});
		StateMachine->CreateState("Land", [this]() {
			return ApplyBirdAnim("A_BirdBackFly", 1.f, 1.f, false, true);
			});
	}

	/* To Idle */
	{
		StateMachine->AddTransition("Peck", "Idle");
		StateMachine->AddTransition("Land", "Idle");
		StateMachine->AddTransition("Cry", "Idle");
	}

	/* To Wait */
	{
		auto Lamda = [this](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
			std::shared_ptr<CBird> Bird = std::static_pointer_cast<CBird>(Comp->GetOwner().lock());
			if (Bird != nullptr)
			{
				if (Bird->mAnimStateMachine->GetCurStateAccTime() > mWaitAnimTime)
				{
					mWaitAnimTime = std::rand() % 4 + 2.f;
					return true;
				}
			}
			return false;
			};
		StateMachine->AddTransition("Idle", "Peck", Lamda, -1);
	}

	/* To Fly */
	{
		StateMachine->AddTransition("FlyStart", "Fly");
	}

	/* To Land */
	{
		auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
			std::shared_ptr<CBird> Bird = std::static_pointer_cast<CBird>(Comp->GetOwner().lock());
			if (Bird != nullptr)
			{
				return Bird->mMovementComponent->IsActiveMovement("AIFly") == false;
			}
			return false;
			};
		StateMachine->AddTransition("Fly", "Land", Lamda);
	}
}

void CBird::Cry()
{
	mAnimStateMachine->ForcedTransition("Cry");
}

void CBird::Fly(const FVector3& MovePos)
{
	mOnFlyEndCallback = nullptr;
	mMovementComponent->DoMovement("AIFly", &MovePos);
}

void CBird::Fly(const FVector3& MovePos, std::function<void()> Callback)
{
	mOnFlyEndCallback = Callback;
	mMovementComponent->DoMovement("AIFly", &MovePos);
}

void CBird::OnFlyEnd()
{
	if (mOnFlyEndCallback != nullptr)
	{
		mOnFlyEndCallback();
		mOnFlyEndCallback = nullptr;
	}
}

void CBird::StartTalk(const std::wstring& MainText, const std::wstring& SubText, const std::string TextureName)
{
	std::shared_ptr<CSimpleTalkWidget> Widget = mWidgetComponent->GetWidget<CSimpleTalkWidget>();
	if (Widget == nullptr)
	{
		return;
	}
	Widget->ChangeText(MainText, SubText, TextureName);
}

void CBird::StopTalk()
{
	std::shared_ptr<CSimpleTalkWidget> Widget = mWidgetComponent->GetWidget<CSimpleTalkWidget>();
	if (Widget == nullptr)
	{
		return;
	}
	Widget->ChangeText(TEXT(""), TEXT(""), "");
}

void CBird::InitCharacterMovements()
{
	if (mMovementComponent != nullptr)
	{
		mMovementComponent->SetSharedState(std::make_shared<FSharedGameState>(mSelf));

		/* 날기 */
		{
			std::shared_ptr<CAIFlyMovement> Movement = std::make_shared<CAIFlyMovement>();
			if (Movement != nullptr)
			{
				Movement->mOnFly.push_back([this](const FVector3& MoveDir) {
					mAnimStateMachine->ForcedTransition("FlyStart", FAnimSequenceOptionOverride{
						1.f,
						0.8f,
						false,
						false,
						mAnimStateMachine->GetLastAnimSymmetry(),
						false
						});
					}
				);
				Movement->mOnFinish.push_back([this]()
					{
						OnFlyEnd();
					});
				mMovementComponent->AddMovement(Movement);
			}
		}
	}
}

FAnimStateData CBird::ApplyBirdAnim(const std::string& Name, float PlayTime, float PlayRate, bool IsLoop, bool IsReverse, bool IsSymmetry, bool IsSymmetryOnSight)
{
	FAnimStateData Data
	{
		Name,
		FAnimSequenceOptionOverride
		{
			PlayTime,
			PlayRate,
			IsLoop,
			IsReverse,
			IsSymmetry,
			IsSymmetryOnSight
		}
	};
	return Data;
}