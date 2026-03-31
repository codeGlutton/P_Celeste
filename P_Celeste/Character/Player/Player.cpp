#include "Pch.h"
#include "Player.h"

#include "Component/MeshComponent.h"
#include "Component/Animation2DComponent.h"
#include "Component/HairMeshComponent.h"
#include "Component/ColliderBox2D.h"
#include "Component/CharacterMovementComponent.h"
#include "Component/TileMapCameraComponent.h"

#include "Movement/PlayerGameState.h"

#include "Movement/WalkMovement.h"
#include "Movement/WallJumpMovement.h"
#include "Movement/PlayerJumpMovement.h"
#include "Movement/DashMovement.h"
#include "Movement/DangleMovement.h"
#include "Movement/ClimbMovement.h"
#include "Movement/SlideMovement.h"
#include "Movement/FastFallMovement.h"
#include "Movement/DeadMovement.h"
#include "Movement/DialogueMovement.h"

#include "World/WorldCollision.h"
#include "World/TileMainWorld.h"
#include "World/Input.h"
#include "Device.h"

#include "UI/Main/ScreenSwapWidget.h"

CPlayer::CPlayer()
{
	SetClassType<CPlayer>();
}

bool CPlayer::Init()
{
	CCharacter::Init();

	InitCheckBoxCollisions();
	InitCharacterMovements();
	InitDialogueBrushes();

	if (mMovementComponent != nullptr)
	{
		mMovementComponent->SetMoveTestComponent(mMoveTestBoxComponent);
		mMovementComponent->SetSpeed(320.f);
		mMovementComponent->SetAccel(3200.f);
	}

	if (mMeshComponent != nullptr)
	{
		mMeshComponent->SetRelativePos(0.f, 30.f);
	}

	mBreathMeshComponent = CreateComponent<CMeshComponent>("BreathMesh").lock();
	if (mBreathMeshComponent != nullptr)
	{
		mBreathMeshComponent->SetShader("DefaultTexture2D");
		mBreathMeshComponent->SetMesh("RectTex");
		mBreathMeshComponent->SetRelativeScale(50.f, 50.f);
		mBreathMeshComponent->SetRelativePos(0.f, 20.f);
		mBreathMeshComponent->SetMaterialBaseColor(0, 255, 255, 255, 255);
		mBreathMeshComponent->SetBlendState(0, "AlphaBlend");
		mBreathMeshComponent->SetRenderLayer("PlayerFrontSubObject");
		mBreathMeshComponent->SetPivot(0.5f, 0.5f);
	}

	mBreathAnimation2DComponent = CreateComponent<CAnimation2DComponent>("BreathAnimation2D").lock();
	if (mBreathAnimation2DComponent != nullptr)
	{
		mBreathAnimation2DComponent->SetUpdateComponent(mBreathMeshComponent);

		mBreathAnimation2DComponent->AddAnimation("A_BreathDangle");
		mBreathAnimation2DComponent->SetLoop("A_BreathDangle", true);
		mBreathAnimation2DComponent->AddAnimation("A_BreathWarn");
		mBreathAnimation2DComponent->SetLoop("A_BreathWarn", true);
		mBreathAnimation2DComponent->AddAnimation("A_BreathIdle");
		mBreathAnimation2DComponent->SetLoop("A_BreathIdle", true);

		mBreathAnimation2DComponent->ChangeAnimation("A_BreathIdle");
	}

	mCameraComponent = CreateComponent<CTileMapCameraComponent>("PlayerCamera").lock();
	if (mCameraComponent != nullptr)
	{
		const FResolution& RS = CDevice::GetInst()->GetResolution();
		mCameraComponent->SetProjection(ECameraProjectionType::Ortho, 90.f, RS.Width, RS.Height, 1000.f);
		mCameraComponent->SetInheritScale(false);
		mCameraComponent->SetInheritRot(false);
	}

	mHairMeshComp = CreateComponent<CHairMeshComponent>("HairMesh").lock();
	if (mHairMeshComp != nullptr)
	{
		mHairMeshComp->SetTargetComponent(mMeshComponent);
		mHairMeshComp->SetPhysicsComponent(mBodyComponent);
		mHairMeshComp->SetTexture(EHairTextureType::FrontHair, "T_PlayerFrontHair");
		mHairMeshComp->SetTexture(EHairTextureType::BackHair, "T_PlayerBackHair");
		mHairMeshComp->SetRenderLayer("PlayerBackSubObject");
		mHairMeshComp->SetPivot(0.5f, 0.5f);
		mHairMeshComp->SetHairColor(CHairMeshComponent::mStaticHairColors[1]);
	}

#ifdef _DEBUG

	mDebugCameraComponent = CreateComponent<CCameraComponent>("DebugCamera").lock();
	if (mDebugCameraComponent != nullptr)
	{
		const FResolution& RS = CDevice::GetInst()->GetResolution();
		float ZoomRate = mWorld.lock()->GetCameraManager().lock()->GetMainCameraZoomRate();
		mDebugCameraComponent->SetProjection(ECameraProjectionType::Ortho, 90.f, RS.Width * ZoomRate, RS.Height * ZoomRate, 1000.f);
		mDebugCameraComponent->SetInheritScale(false);
		mDebugCameraComponent->SetInheritRot(false);
		mDebugCameraComponent->Zoom(3.f);
	}

#endif

	mScreenSwapWidget = mWorld.lock()->GetUIManager().lock()->CreateWidget<CScreenSwapWidget>("ScreenSwap", 2).lock();

	return true;
}

void CPlayer::Begin()
{
	CCharacter::Begin();
}

void CPlayer::Update(float DeltaTime)
{
	CCharacter::Update(DeltaTime);
}

void CPlayer::PostUpdate(float DeltaTime)
{
	CCharacter::PostUpdate(DeltaTime);

	mHairMeshComp->SetSymmetry(mAnimStateMachine->GetLastAnimSymmetry());
	mBreathAnimation2DComponent->SetSymmetry(mBreathAnimation2DComponent->GetAnimationSplitName(), mAnimStateMachine->GetLastAnimSymmetry());

	if (mIsMeshWarn == true)
	{
		mWarnFlickerAccTime += DeltaTime;
		while (mWarnFlickerAccTime > mWarnFlickerFrameTime)
		{
			mWarnFlickerAccTime -= mWarnFlickerFrameTime;

			mMeshComponent->SetMaterialBaseColor(0, mWarningColor[mWarningIndex]);
			mWarningIndex = (mWarningIndex + 1) % mWarningColor.size();
		}
	}
	else
	{
		mMeshComponent->SetMaterialBaseColor(0, FVector4::White);
	}

	if (mMovementComponent->IsCrushed() == true)
	{
		TakeDamage(-1.f);
	}
}

void CPlayer::Destroy()
{
	CCharacter::Destroy();
}

float CPlayer::TakeDamage(float Damage)
{
	if (Damage < 0.f)
	{
		mMovementComponent->DoMovement("Dead", &mRespawnDelayTime);
	}
	return CGameObject::TakeDamage(Damage);
}

void CPlayer::ForcedRechageMovements()
{
	mMovementComponent->GetMovement<CDashMovement>(mDashHandle)->RechargeAllPoint();
	mMovementComponent->GetMovement<CDangleMovement>(mDangleHandle)->RechargeAllPoint();
}

std::shared_ptr<CCollider> CPlayer::MakeRootCollider()
{
	std::shared_ptr<CColliderBox2D> Body = CreateComponent<CColliderBox2D>("Body").lock();
	if (Body != nullptr)
	{
		Body->SetCollisionProfile("Player");
		Body->SetBoxSize(40.f, 40.f);

#ifdef _DEBUG

		Body->SetDebugDraw(true);

#endif // _DEBUG

		Body->SetInheritScale(true);
		Body->SetRelativePos(0.f, 0.f);

		Body->SetSimulatePhysics(true);
		Body->SetUseGravity(true);
		Body->SetGravitySacle(2.f);
		Body->SetUseMaxDropVelocity(true);
		Body->SetMaxDropVelocity(700.f);
	}
	return Body;
}

void CPlayer::BindPlayerInputs(std::shared_ptr<CInput>& InputManager)
{
	{
		InputManager->AddBindKey("MoveUp", VK_UP);
		InputManager->SetBindFunction<CPlayer>("MoveUp", EInputType::Hold, this, &CPlayer::MoveUp);

		InputManager->AddBindKey("MoveDown", VK_DOWN);
		InputManager->SetBindFunction<CPlayer>("MoveDown", EInputType::Hold, this, &CPlayer::MoveDown);
		InputManager->SetBindFunction<CPlayer>("MoveDown", EInputType::Press, this, &CPlayer::MoveFastFall);
		InputManager->SetBindFunction<CPlayer>("MoveDown", EInputType::Release, this, &CPlayer::MoveSlowFall);

		InputManager->AddBindKey("MoveLeft", VK_LEFT);
		InputManager->SetBindFunction<CPlayer>("MoveLeft", EInputType::Hold, this, &CPlayer::MoveLeft);

		InputManager->AddBindKey("MoveRight", VK_RIGHT);
		InputManager->SetBindFunction<CPlayer>("MoveRight", EInputType::Hold, this, &CPlayer::MoveRight);
	}

	{
		InputManager->AddBindKey("Jump", 'C');
		InputManager->SetBindFunction<CPlayer>("Jump", EInputType::Press, this, &CPlayer::MoveJump);
		InputManager->SetBindFunction<CPlayer>("Jump", EInputType::Release, this, &CPlayer::MoveDrop);

		InputManager->AddBindKey("Dash", 'Z');
		InputManager->SetBindFunction<CPlayer>("Dash", EInputType::Press, this, &CPlayer::MoveDash);
	}

	{
		InputManager->AddBindKey("Dangle", 'X');
		InputManager->SetBindFunction<CPlayer>("Dangle", EInputType::Press, this, &CPlayer::MoveDangle);
		InputManager->SetBindFunction<CPlayer>("Dangle", EInputType::Hold, this, &CPlayer::TryToMoveDangle);
		InputManager->SetBindFunction<CPlayer>("Dangle", EInputType::Release, this, &CPlayer::MoveRelease);
	}

#ifdef _DEBUG

	{
		InputManager->AddBindKey("ChangeCamera", VK_F1);
		InputManager->SetBindFunction<CPlayer>("ChangeCamera", EInputType::Press, this, &CPlayer::ChangeCamera);

		InputManager->AddBindKey("ChangeCollisionTreeDraw", VK_F2);
		InputManager->SetBindFunction<CPlayer>("ChangeCollisionTreeDraw", EInputType::Press, this, &CPlayer::ChangeCollisionTreeDraw);

		InputManager->AddBindKey("ChangeCollisionDraw", VK_F3);
		InputManager->SetBindFunction<CPlayer>("ChangeCollisionDraw", EInputType::Press, this, &CPlayer::ChangeCollisionDraw);

		InputManager->AddBindKey("ChangeHairDraw", VK_F4);
		InputManager->SetBindFunction<CPlayer>("ChangeHairDraw", EInputType::Press, this, &CPlayer::ChangeHairDraw);
	}

#endif // _DEBUG

}

void CPlayer::ConfigStateMachine(std::shared_ptr<CAnimStateMachine>& StateMachine)
{
	if (mAnimation2DComponent != nullptr)
	{
		mAnimation2DComponent->AddAnimation("A_PlayerIdle");

		mAnimation2DComponent->AddAnimation("A_PlayerWalk");
		mAnimation2DComponent->AddNotify("A_PlayerWalk", "E_PlayerWalkStep0", 0, [World = mWorld]() {
			int32 Idx = std::rand() % 7;
			World.lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerWalk" + std::to_string(Idx));
			});
		mAnimation2DComponent->AddNotify("A_PlayerWalk", "E_PlayerWalkStep1", 6, [World = mWorld]() {
			int32 Idx = std::rand() % 7;
			World.lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerWalk" + std::to_string(Idx));
			});

		mAnimation2DComponent->AddAnimation("A_PlayerFlip");
		mAnimation2DComponent->AddNotify("A_PlayerFlip", "E_PlayerWalkStep1", 0, [World = mWorld]() {
			int32 Idx = std::rand() % 7;
			World.lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerWalk" + std::to_string(Idx));
			});

		mAnimation2DComponent->AddAnimation("A_PlayerWait0");
		mAnimation2DComponent->AddAnimation("A_PlayerWait1");
		mAnimation2DComponent->AddAnimation("A_PlayerWait2");

		mAnimation2DComponent->AddAnimation("A_PlayerEdge");
		mAnimation2DComponent->AddAnimation("A_PlayerEdgeBack");

		mAnimation2DComponent->AddAnimation("A_PlayerWallJump");
		mAnimation2DComponent->AddAnimation("A_PlayerJump");
		mAnimation2DComponent->AddAnimation("A_PlayerFall");
		mAnimation2DComponent->AddAnimation("A_PlayerLand");

		mAnimation2DComponent->AddAnimation("A_PlayerDash");

		mAnimation2DComponent->AddAnimation("A_PlayerDangle");
		mAnimation2DComponent->AddAnimation("A_PlayerDangleBackStart");
		mAnimation2DComponent->AddAnimation("A_PlayerDangleBack");

		mAnimation2DComponent->AddAnimation("A_PlayerDangleEdge");

		mAnimation2DComponent->AddAnimation("A_PlayerClimb");
		mAnimation2DComponent->AddNotify("A_PlayerClimb", "E_PlayerClimbStep0", 0, [World = mWorld]() {
			int32 Idx = std::rand() % 5;
			World.lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerClimb" + std::to_string(Idx));
			});
		mAnimation2DComponent->AddNotify("A_PlayerClimb", "E_PlayerClimbStep1", 3, [World = mWorld]() {
			int32 Idx = std::rand() % 5;
			World.lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerClimb" + std::to_string(Idx));
			});

		mAnimation2DComponent->AddAnimation("A_PlayerLookUp");
		mAnimation2DComponent->AddAnimation("A_PlayerLookDown");
		mAnimation2DComponent->AddNotify("A_PlayerLookDown", "E_PlayerLookDown", 0, [World = mWorld]() {
			int32 Idx = std::rand() % 3;
			World.lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerDuck" + std::to_string(Idx));
			});

		mAnimation2DComponent->AddAnimation("A_PlayerPush");
		mAnimation2DComponent->AddNotify("A_PlayerPush", "E_PlayerWalkStep0", 4, [World = mWorld]() {
			int32 Idx = std::rand() % 7;
			World.lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerWalk" + std::to_string(Idx));
			});
		mAnimation2DComponent->AddNotify("A_PlayerPush", "E_PlayerWalkStep1", 13, [World = mWorld]() {
			int32 Idx = std::rand() % 7;
			World.lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerWalk" + std::to_string(Idx));
			});

		mAnimation2DComponent->AddAnimation("A_PlayerDead");
	}

	{
		StateMachine->CreateState("Idle", [this]() {
			return ApplyPlayerAnim("A_PlayerIdle", 1.f, 1.f, true);
			}, true);
		StateMachine->CreateState("Walk", [this]() {
			return ApplyPlayerAnim("A_PlayerWalk", 1.f, 1.f, true);
			});

		StateMachine->CreateState("Flip", [this]() {
			return ApplyPlayerAnim("A_PlayerFlip", 1.f, 2.f, false);
			});

		StateMachine->CreateState("Wait", [this]() {
			int32 WaitIndex = std::rand() % 3;
			return ApplyPlayerAnim("A_PlayerWait" + std::to_string(WaitIndex), 1.f, 1.f, false);
			});

		StateMachine->CreateState("LookUp", [this]() {
			return ApplyPlayerAnim("A_PlayerLookUp", 1.f, 1.f, false);
			});
		StateMachine->CreateState("LookDown", [this]() {
			return ApplyPlayerAnim("A_PlayerLookDown", 1.f, 1.f, false);
			});

		StateMachine->CreateState("Edge", [this]() {
			return ApplyPlayerAnim("A_PlayerEdge", 1.f, 0.5f, true);
			});
		StateMachine->CreateState("EdgeBack", [this]() {
			return ApplyPlayerAnim("A_PlayerEdgeBack", 1.f, 0.5f, true);
			});

		StateMachine->CreateState("WallJump", [this]() {
			return ApplyPlayerAnim("A_PlayerWallJump", 1.f, 0.6f, false);
			});

		StateMachine->CreateState("Jump", [this]() {
			if (mMovementComponent->GetLastAccSight().IsZero() == false)
			{
				return ApplyPlayerAnim("A_PlayerWallJump", 1.f, 1.f, false);
			}
			else
			{
				return ApplyPlayerAnim("A_PlayerJump", 1.f, 1.f, false);
			}
			});
		StateMachine->CreateState("Fall", [this]() {
			return ApplyPlayerAnim("A_PlayerFall", 1.f, 4.f, false);
			});
		StateMachine->CreateState("Land", [this]() {
			return ApplyPlayerAnim("A_PlayerLand", 1.f, 2.f, false);
			});

		StateMachine->CreateState("Dash", [this]() {
			return ApplyPlayerAnim("A_PlayerDash", 1.f, 1.f, false);
			});

		StateMachine->CreateState("Push", [this]() {
			return ApplyPlayerAnim("A_PlayerPush", 1.f, 1.f, true);
			});

		StateMachine->CreateState("Slide", [this]() {
			return ApplyPlayerAnim("A_PlayerDangle", 1.f, 1.f, true);
			});

		StateMachine->CreateState("Dangle", [this]() {
			return ApplyPlayerAnim(
				"A_PlayerDangle",
				1.f,
				1.f,
				true,
				false,
				mMovementComponent->GetSharedState<FPlayerGameState>().GetFixedSymmetry(),
				false
			);
			});
		StateMachine->CreateState("DangleBackStart", [this]() {
			return ApplyPlayerAnim(
				"A_PlayerDangleBackStart",
				1.f,
				0.8f, 
				false,
				false, 
				mMovementComponent->GetSharedState<FPlayerGameState>().GetFixedSymmetry(),
				false
			);
			});
		StateMachine->CreateState("DangleBack", [this]() {
			return ApplyPlayerAnim(
				"A_PlayerDangleBack",
				1.f,
				1.f,
				true,
				false,
				mMovementComponent->GetSharedState<FPlayerGameState>().GetFixedSymmetry(),
				false
			);
			});

		StateMachine->CreateState("DangleEdge", [this]() {
			return ApplyPlayerAnim(
				"A_PlayerDangleEdge",
				1.f, 
				1.f, 
				true,
				false,
				mMovementComponent->GetSharedState<FPlayerGameState>().GetFixedSymmetry(),
				false
			);
			});

		StateMachine->CreateState("ClimbUp", [this]() {
			return ApplyPlayerAnim(
				"A_PlayerClimb",
				1.f,
				1.6f,
				true,
				false,
				mMovementComponent->GetSharedState<FPlayerGameState>().GetFixedSymmetry(),
				false
			);
			});

		StateMachine->CreateState("Dead", [this]() {
			return ApplyPlayerAnim("A_PlayerDead", 1.f, 1.5f, false, false, mAnimStateMachine->GetLastAnimSymmetry(), false);
			});
	}
	{
		static const float StaticThreshold = 50.f;

		/* To Walk */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->GetSpeed() > StaticThreshold * DeltaTime &&
						Player->mMovementComponent->IsLandingOnWall() == true;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Walk", Lamda);
			StateMachine->AddTransition("Wait", "Walk", Lamda, 1);
			StateMachine->AddTransition("Edge", "Walk", Lamda, 1);
			StateMachine->AddTransition("EdgeBack", "Walk", Lamda, 1);
			StateMachine->AddTransition("LookUp", "Walk", Lamda, 1);
			StateMachine->AddTransition("LookDown", "Walk", Lamda, 1);
			auto LandLamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Comp->GetAnimationFrame() > 2 && Player->GetSpeed() > StaticThreshold * DeltaTime &&
						Player->mMovementComponent->IsLandingOnWall() == true;
				}
				return false;
				};
			StateMachine->AddTransition("Land", "Walk", LandLamda, 1);
			StateMachine->AddTransition("Flip", "Walk", -1);
		}

		/* To Flip */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					if (Player->mMovementComponent->IsLandingOnWall() == true)
					{
						if ((Player->mMovementComponent->GetLastHorizonSight().x * Player->mMovementComponent->GetHorizonSight().x) < 0.f)
						{
							return true;
						}
					}
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Flip", Lamda, 1);
			StateMachine->AddTransition("Walk", "Flip", Lamda);
		}

		/* To Idle */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->GetSpeed() <= StaticThreshold * DeltaTime &&
						Player->mMovementComponent->IsLandingOnWall() == true;
				}
				return false;
				};
			StateMachine->AddTransition("Walk", "Idle", Lamda);
			StateMachine->AddTransition("Jump", "Idle", Lamda);
			StateMachine->AddTransition("WallJump", "Idle", Lamda);
			StateMachine->AddTransition("Push", "Idle", Lamda);
			StateMachine->AddTransition("Wait", "Idle");
			StateMachine->AddTransition("Land", "Idle");
			StateMachine->AddTransition("Dash", "Idle");

			auto EdgeLamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mLeftFootCheckComponent->HasCollisionObject() == Player->mRightFootCheckComponent->HasCollisionObject() &&
						Player->mLeftFootCheckComponent->HasCollisionObject() == true;
				}
				return false;
				};
			StateMachine->AddTransition("Edge", "Idle", EdgeLamda);
			StateMachine->AddTransition("EdgeBack", "Idle", EdgeLamda);

			auto DangleLamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->IsActiveMovement("Dangle") == false;
				}
				return false;
				};
			StateMachine->AddTransition("Dangle", "Idle", DangleLamda, 1);
			StateMachine->AddTransition("ClimbUp", "Idle", DangleLamda, 1);
			StateMachine->AddTransition("DangleEdge", "Idle", DangleLamda, 1);
			StateMachine->AddTransition("DangleBackStart", "Idle", DangleLamda, 1);
			StateMachine->AddTransition("DangleBack", "Idle", DangleLamda, 1);

			auto LookLamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->GetSpeed() <= StaticThreshold * DeltaTime &&
						Player->mMovementComponent->IsLandingOnWall() == true &&
						Player->mMovementComponent->GetLastAccSight().y == 0.f;
				}
				return false;
				};
			StateMachine->AddTransition("LookUp", "Idle", LookLamda);
			StateMachine->AddTransition("LookDown", "Idle", LookLamda);
		}

		/* To Wait */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mAnimStateMachine->GetCurStateAccTime() > 4.f;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Wait", Lamda, -3);
		}

		/* To LookUp */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->GetSpeed() <= StaticThreshold * DeltaTime &&
						Player->mMovementComponent->IsLandingOnWall() == true &&
						Player->mMovementComponent->GetLastAccSight().y > 0.f;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "LookUp", Lamda);
		}

		/* To LookDown */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->GetSpeed() <= StaticThreshold * DeltaTime &&
						Player->mMovementComponent->IsLandingOnWall() == true &&
						Player->mMovementComponent->GetLastAccSight().y < 0.f;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "LookDown", Lamda);
			StateMachine->AddTransition("Edge", "LookDown", Lamda);
			StateMachine->AddTransition("EdgeBack", "LookDown", Lamda);
		}

		/* To Edge */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mLeftFootCheckComponent->HasCollisionObject() != Player->mRightFootCheckComponent->HasCollisionObject() &&
						Player->mLeftFootCheckComponent->HasCollisionObject() == Player->mMovementComponent->GetHorizonSight().x > 0;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Edge", Lamda, -1);
			StateMachine->AddTransition("EdgeBack", "Edge", Lamda);
			StateMachine->AddTransition("Flip", "Edge", Lamda);
		}

		/* To EdgeBack */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mLeftFootCheckComponent->HasCollisionObject() != Player->mRightFootCheckComponent->HasCollisionObject() &&
						Player->mLeftFootCheckComponent->HasCollisionObject() != Player->mMovementComponent->GetHorizonSight().x > 0;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "EdgeBack", Lamda);
			StateMachine->AddTransition("Edge", "EdgeBack", Lamda);
			StateMachine->AddTransition("Flip", "EdgeBack", Lamda);
		}

		/* To Jump */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->GetVelocity().y > StaticThreshold * DeltaTime &&
						Player->mMovementComponent->IsLandingOnWall() == false;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Jump", Lamda, 2);
			StateMachine->AddTransition("Wait", "Jump", Lamda, 2);
			StateMachine->AddTransition("Walk", "Jump", Lamda, 2);
			StateMachine->AddTransition("Flip", "Jump", Lamda, 2);
			StateMachine->AddTransition("Land", "Jump", Lamda, 2);
			StateMachine->AddTransition("Edge", "Jump", Lamda, 2);
			StateMachine->AddTransition("EdgeBack", "Jump", Lamda, 2);
			StateMachine->AddTransition("LookUp", "Jump", Lamda, 2);
			StateMachine->AddTransition("LookDown", "Jump", Lamda, 2);
			StateMachine->AddTransition("Push", "Jump", Lamda, 2);
		}
		
		/* To Fall */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->GetVelocity().y < -StaticThreshold * DeltaTime &&
						Player->mMovementComponent->IsLandingOnWall() == false;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Fall", Lamda, 2);
			StateMachine->AddTransition("Wait", "Fall", Lamda, 2);
			StateMachine->AddTransition("Walk", "Fall", Lamda, 2);
			StateMachine->AddTransition("Flip", "Fall", Lamda, 2);
			StateMachine->AddTransition("Land", "Fall", Lamda, 2);
			StateMachine->AddTransition("Jump", "Fall", Lamda, 2);
			StateMachine->AddTransition("WallJump", "Fall", Lamda, 2);
			StateMachine->AddTransition("Edge", "Fall", Lamda, 2);
			StateMachine->AddTransition("EdgeBack", "Fall", Lamda, 2);
			StateMachine->AddTransition("LookUp", "Fall", Lamda, 2);
			StateMachine->AddTransition("LookDown", "Fall", Lamda, 2);
			StateMachine->AddTransition("Push", "Fall", Lamda, 2);

			auto SlideLamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->IsActiveMovement("Slide") == false &&
						Player->mMovementComponent->IsLandingOnWall() == false;
				}
				return false;
				};
			StateMachine->AddTransition("Slide", "Fall", SlideLamda, 2);
		}

		/* To Land */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->IsLandingOnWall() == true;
				}
				return false;
				};
			StateMachine->AddTransition("WallJump", "Land", Lamda);
			StateMachine->AddTransition("Jump", "Land", Lamda);
			StateMachine->AddTransition("Fall", "Land", Lamda);
			StateMachine->AddTransition("Slide", "Land", Lamda);

			auto DeadLamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->IsActiveMovement("Dead") == false;
				}
				return false;
				};
			StateMachine->AddTransition("Dead", "Land", DeadLamda);
		}

		/* To Dash */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->IsActiveMovement("Dash");
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Dash", Lamda, 3);
			StateMachine->AddTransition("Wait", "Dash", Lamda, 3);
			StateMachine->AddTransition("Walk", "Dash", Lamda, 3);
			StateMachine->AddTransition("Flip", "Dash", Lamda, 3);
			StateMachine->AddTransition("Jump", "Dash", Lamda, 3);
			StateMachine->AddTransition("WallJump", "Dash", Lamda, 3);
			StateMachine->AddTransition("Fall", "Dash", Lamda, 3);
			StateMachine->AddTransition("Land", "Dash", Lamda, 3);
			StateMachine->AddTransition("Edge", "Dash", Lamda, 3);
			StateMachine->AddTransition("EdgeBack", "Dash", Lamda, 3);
			StateMachine->AddTransition("LookUp", "Dash", Lamda, 3);
			StateMachine->AddTransition("LookDown", "Dash", Lamda, 3);
			StateMachine->AddTransition("Slide", "Dash", Lamda, 3);
			StateMachine->AddTransition("Push", "Dash", Lamda, 3);
		}

		/* To Push */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					const std::shared_ptr<CCharacterMovementComponent>& MoveComp = Player->mMovementComponent;
					const std::shared_ptr<CCollider>& UpdateComp = Player->mBodyComponent;
					const FSharedGameState& SharedState = MoveComp->GetSharedState();
					if (MoveComp->GetLastMoveDir().IsZero() == true || MoveComp->IsLandingOnWall() == false)
					{
						return false;
					}

					return (MoveComp->IsSideOfWall() == true) &&
						(MoveComp->GetHorizonSight().x < 0.f) == ((SharedState.GetWallState() & ETouchedWall::Flag::Left) != 0) &&
						(UpdateComp->GetPhysicsVelocity().y < 0.f);
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Push", Lamda);
			StateMachine->AddTransition("Wait", "Push", Lamda);
			StateMachine->AddTransition("Walk", "Push", Lamda);
			StateMachine->AddTransition("Flip", "Push", Lamda);
			StateMachine->AddTransition("Jump", "Push", Lamda);
			StateMachine->AddTransition("WallJump", "Push", Lamda);
			StateMachine->AddTransition("Fall", "Push", Lamda);
			StateMachine->AddTransition("Land", "Push", Lamda);
			StateMachine->AddTransition("Edge", "Push", Lamda);
			StateMachine->AddTransition("EdgeBack", "Push", Lamda);
			StateMachine->AddTransition("LookUp", "Push", Lamda);
			StateMachine->AddTransition("LookDown", "Push", Lamda);
			StateMachine->AddTransition("Slide", "Push", Lamda);
		}

		/* To Slide */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->IsActiveMovement("Slide") == true;
				}
				return false;
				};
			StateMachine->AddTransition("Jump", "Slide", Lamda, 3);
			StateMachine->AddTransition("WallJump", "Slide", Lamda, 3);
			StateMachine->AddTransition("Fall", "Slide", Lamda, 3);
			StateMachine->AddTransition("Dangle", "Slide", Lamda, 3);
			StateMachine->AddTransition("Push", "Slide", Lamda, 3);
		}

		/* To Dangle */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->IsActiveMovement("Dangle") == true;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Dangle", Lamda, 3);
			StateMachine->AddTransition("Wait", "Dangle", Lamda, 3);
			StateMachine->AddTransition("Walk", "Dangle", Lamda, 3);
			StateMachine->AddTransition("Flip", "Dangle", Lamda, 3);
			StateMachine->AddTransition("Jump", "Dangle", Lamda, 3);
			StateMachine->AddTransition("WallJump", "Dangle", Lamda, 3);
			StateMachine->AddTransition("Fall", "Dangle", Lamda, 3);
			StateMachine->AddTransition("Land", "Dangle", Lamda, 3);
			StateMachine->AddTransition("Edge", "Dangle", Lamda, 3);
			StateMachine->AddTransition("EdgeBack", "Dangle", Lamda, 3);
			StateMachine->AddTransition("LookUp", "Dangle", Lamda, 3);
			StateMachine->AddTransition("LookDown", "Dangle", Lamda, 3);
			StateMachine->AddTransition("Slide", "Dangle", Lamda, 3);
			StateMachine->AddTransition("Push", "Dangle", Lamda, 3);

			auto DangleBackLamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					const std::shared_ptr<CCharacterMovementComponent>& MovementComp = Player->mMovementComponent;
					const FVector3& LastAccSight = MovementComp->GetLastAccSight();
					if (MovementComp->GetSharedState<FPlayerGameState>().GetFixedSymmetry() == true)
					{
						return LastAccSight.x < 0.99f;
					}
					else
					{
						return LastAccSight.x > -0.99f;
					}
				}
				return false;
				};
			StateMachine->AddTransition("DangleBack", "Dangle", DangleBackLamda);
			StateMachine->AddTransition("DangleBackStart", "Dangle", DangleBackLamda);

			auto ClimbLamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->GetMoveDir().y <= 0.f && Player->mMovementComponent->IsActiveMovement("Dangle") == true;
				}
				return false;
				};
			StateMachine->AddTransition("ClimbUp", "Dangle", ClimbLamda);
		}

		/* To DangleEdge */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					if (Player->mMovementComponent->GetSharedState<FPlayerGameState>().GetFixedSymmetry() == true)
					{
						return Player->mLeftWallCheckComponent->HasCollisionObject() == false;
					}
					else
					{
						return Player->mRightWallCheckComponent->HasCollisionObject() == false;
					}
				}
				return false;
				};
			StateMachine->AddTransition("Dangle", "DangleEdge", Lamda);
			StateMachine->AddTransition("DangleBack", "DangleEdge", Lamda);
			StateMachine->AddTransition("DangleBackStart", "DangleEdge", Lamda);
		}

		/* To DangleBackStart */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					const std::shared_ptr<CCharacterMovementComponent>& MovementComp = Player->mMovementComponent;
					const FVector3& LastAccSight = MovementComp->GetLastAccSight();
					if (MovementComp->GetSharedState<FPlayerGameState>().GetFixedSymmetry() == true)
					{
						return LastAccSight.x >= 0.99f;
					}
					else
					{
						return LastAccSight.x <= -0.99f;
					}
				}
				return false;
				};
			StateMachine->AddTransition("Dangle", "DangleBackStart", Lamda);
		}

		/* To DangleBack */
		{
			StateMachine->AddTransition("DangleBackStart", "DangleBack");
		}

		/* To ClimbUp */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->GetMoveDir().y > 0.f && Player->mMovementComponent->IsActiveMovement("Dangle") == true;
				}
				return false;
				};
			StateMachine->AddTransition("Dangle", "ClimbUp", Lamda);
			StateMachine->AddTransition("DangleBack", "ClimbUp", Lamda);
			StateMachine->AddTransition("DangleBackStart", "ClimbUp", Lamda);
			StateMachine->AddTransition("DangleEdge", "ClimbUp", Lamda);
			StateMachine->AddTransition("ClimbDown", "ClimbUp", Lamda);
		}

		/* To Dead */
		{
			auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
				std::shared_ptr<CPlayer> Player = std::static_pointer_cast<CPlayer>(Comp->GetOwner().lock());
				if (Player != nullptr)
				{
					return Player->mMovementComponent->IsActiveMovement("Dead") == true;
				}
				return false;
				};
			StateMachine->AddTransition("Idle", "Dead", Lamda);
			StateMachine->AddTransition("Walk", "Dead", Lamda);
			StateMachine->AddTransition("Flip", "Dead", Lamda);
			StateMachine->AddTransition("Wait", "Dead", Lamda);
			StateMachine->AddTransition("LookUp", "Dead", Lamda);
			StateMachine->AddTransition("LookDown", "Dead", Lamda);
			StateMachine->AddTransition("Edge", "Dead", Lamda);
			StateMachine->AddTransition("EdgeBack", "Dead", Lamda);
			StateMachine->AddTransition("WallJump", "Dead", Lamda);
			StateMachine->AddTransition("Jump", "Dead", Lamda);
			StateMachine->AddTransition("Fall", "Dead", Lamda);
			StateMachine->AddTransition("Land", "Dead", Lamda);
			StateMachine->AddTransition("Dash", "Dead", Lamda);
			StateMachine->AddTransition("Push", "Dead", Lamda);
			StateMachine->AddTransition("Slide", "Dead", Lamda);
			StateMachine->AddTransition("Dangle", "Dead", Lamda);
			StateMachine->AddTransition("DangleBackStart", "Dead", Lamda);
			StateMachine->AddTransition("DangleBack", "Dead", Lamda);
			StateMachine->AddTransition("DangleEdge", "Dead", Lamda);
			StateMachine->AddTransition("ClimbUp", "Dead", Lamda);
		}
	}
}

std::shared_ptr<FDialoguePanelBrush> CPlayer::GetDialoguePanelBrush()
{
	return mDialoguePanelBrush;
}

std::shared_ptr<FDialoguePortraitBrush> CPlayer::GetDialoguePortraitBrush(const std::string& EmotionName)
{
	if (mDialoguePortraitBrushes.find(EmotionName) == mDialoguePortraitBrushes.end())
	{
		return nullptr;
	}
	return mDialoguePortraitBrushes.at(EmotionName);
}

std::shared_ptr<CCameraComponent> CPlayer::GetCameraComp()
{
	return mCameraComponent;
}

void CPlayer::InitDialogueBrushes()
{
	mDialoguePanelBrush = std::make_shared<FDialoguePanelBrush>();
	{
		mDialoguePanelBrush->mPanelTexture = "T_PlayerPanel";
		mDialoguePanelBrush->mTint = FVector4::White;
	}

	mDialoguePortraitBrushes.insert({ "Normal", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["Normal"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_PlayerPortraitNormal", 3, 4);
		PortraitBrush->mSounds = GetEmotionSoundNames("S_PlayerPortraitNormal", 10, 3);
		PortraitBrush->mPerSound = { "S_PlayerPortraitPerNormal", 10 };
		PortraitBrush->mIsLoop = false;
	}
	mDialoguePortraitBrushes.insert({ "NormalLoop", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["NormalLoop"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_PlayerPortraitNormal", 4, 0);
		PortraitBrush->mIsLoop = true;
		PortraitBrush->mLoopDelayFrame = 45;
	}
	mDialoguePortraitBrushes.insert({ "Angry", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["Angry"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_PlayerPortraitAngry", 3, 4);
		PortraitBrush->mSounds = GetEmotionSoundNames("S_PlayerPortraitAngry", 10, 3);
		PortraitBrush->mPerSound = { "S_PlayerPortraitPerAngry", 10 };
		PortraitBrush->mIsLoop = false;
	}
	mDialoguePortraitBrushes.insert({ "AngryLoop", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["AngryLoop"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_PlayerPortraitAngry", 4, 0);
		PortraitBrush->mIsLoop = true;
		PortraitBrush->mLoopDelayFrame = 45;
	}
	mDialoguePortraitBrushes.insert({ "Upset", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["Upset"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_PlayerPortraitUpset", 3, 4);
		PortraitBrush->mSounds = GetEmotionSoundNames("S_PlayerPortraitUpset", 10, 3);
		PortraitBrush->mPerSound = { "S_PlayerPortraitPerUpset", 10 };
		PortraitBrush->mIsLoop = false;
	}
	mDialoguePortraitBrushes.insert({ "UpsetLoop", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["UpsetLoop"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_PlayerPortraitUpset", 4, 0);
		PortraitBrush->mIsLoop = true;
		PortraitBrush->mLoopDelayFrame = 45;
	}
	mDialoguePortraitBrushes.insert({ "Sad", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["Sad"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_PlayerPortraitSad", 3, 4);
		PortraitBrush->mSounds = GetEmotionSoundNames("S_PlayerPortraitSad", 10, 3);
		PortraitBrush->mPerSound = { "S_PlayerPortraitPerSad", 10 };
		PortraitBrush->mIsLoop = false;
	}
	mDialoguePortraitBrushes.insert({ "SadLoop", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["SadLoop"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_PlayerPortraitSad", 4, 0);
		PortraitBrush->mIsLoop = true;
		PortraitBrush->mLoopDelayFrame = 45;
	}
}

void CPlayer::InitCheckBoxCollisions()
{
	mUpWallCheckComponent = CreateComponent<CColliderBox2D>("UpWallCheck").lock();
	if (mUpWallCheckComponent != nullptr)
	{
		mUpWallCheckComponent->SetBoxSize(33.f, 5.f);
		mUpWallCheckComponent->SetRelativePos(0.f, 20.f);
		mUpWallCheckComponent->SetCollisionProfile("EnvSensor");
		mUpWallCheckComponent->SetCollisionBeginFunction([this](const FVector3& HitPos, CCollider* Other) {
			mMovementComponent->GetSharedState().AddTouchedWall(ETouchedWall::Type::Up, Other);
			});
		mUpWallCheckComponent->SetCollisionEndFunction([this](CCollider* Other) {
			mMovementComponent->GetSharedState().RemoveTouchedWall(ETouchedWall::Type::Up, Other);
			});

#ifdef _DEBUG

		mUpWallCheckComponent->SetDebugDraw(true);

#endif // _DEBUG
	}

	mLeftFootCheckComponent = CreateComponent<CColliderBox2D>("LeftFootCheck").lock();
	if (mLeftFootCheckComponent != nullptr)
	{
		mLeftFootCheckComponent->SetBoxSize(13.f, 5.f);
		mLeftFootCheckComponent->SetRelativePos(-10.f, -20.f);
		mLeftFootCheckComponent->SetCollisionProfile("EnvSensor");
		mLeftFootCheckComponent->SetCollisionBeginFunction([this](const FVector3& HitPos, CCollider* Other) {
			mMovementComponent->GetSharedState().AddTouchedWall(ETouchedWall::Type::Down, Other);
			});
		mLeftFootCheckComponent->SetCollisionEndFunction([this](CCollider* Other) {
			mMovementComponent->GetSharedState().RemoveTouchedWall(ETouchedWall::Type::Down, Other);
			});

#ifdef _DEBUG

		mLeftFootCheckComponent->SetDebugDraw(true);

#endif // _DEBUG
	}

	mRightFootCheckComponent = CreateComponent<CColliderBox2D>("RightFootCheck").lock();
	if (mRightFootCheckComponent != nullptr)
	{
		mRightFootCheckComponent->SetBoxSize(13.f, 5.f);
		mRightFootCheckComponent->SetRelativePos(10.f, -20.f);
		mRightFootCheckComponent->SetCollisionProfile("EnvSensor");
		mRightFootCheckComponent->SetCollisionBeginFunction([this](const FVector3& HitPos, CCollider* Other) {
			mMovementComponent->GetSharedState().AddTouchedWall(ETouchedWall::Type::Down, Other);
			});
		mRightFootCheckComponent->SetCollisionEndFunction([this](CCollider* Other) {
			mMovementComponent->GetSharedState().RemoveTouchedWall(ETouchedWall::Type::Down, Other);
			});

#ifdef _DEBUG

		mRightFootCheckComponent->SetDebugDraw(true);

#endif // _DEBUG
	}

	mLeftHandCheckComponent = CreateComponent<CColliderBox2D>("LeftHandCheck").lock();
	if (mLeftHandCheckComponent != nullptr)
	{
		mLeftHandCheckComponent->SetBoxSize(5.f, 18.f);
		mLeftHandCheckComponent->SetRelativePos(-20.f, 9.f);
		mLeftHandCheckComponent->SetCollisionProfile("EnvSensor");
		mLeftHandCheckComponent->SetCollisionBeginFunction([this](const FVector3& HitPos, CCollider* Other) {
			mMovementComponent->GetSharedState().AddTouchedWall(ETouchedWall::Type::Left, Other);
			});
		mLeftHandCheckComponent->SetCollisionEndFunction([this](CCollider* Other) {
			mMovementComponent->GetSharedState().RemoveTouchedWall(ETouchedWall::Type::Left, Other);
			});

#ifdef _DEBUG

		mLeftHandCheckComponent->SetDebugDraw(true);

#endif // _DEBUG
	}

	mLeftWallCheckComponent = CreateComponent<CColliderBox2D>("LeftWallCheck").lock();
	if (mLeftHandCheckComponent != nullptr)
	{
		mLeftWallCheckComponent->SetBoxSize(5.f, 7.f);
		mLeftWallCheckComponent->SetRelativePos(-20.f, -10.f);
		mLeftWallCheckComponent->SetCollisionProfile("EnvSensor");

#ifdef _DEBUG

		mLeftWallCheckComponent->SetDebugDraw(true);

#endif // _DEBUG
	}

	mRightHandCheckComponent = CreateComponent<CColliderBox2D>("RightHandCheck").lock();
	if (mRightHandCheckComponent != nullptr)
	{
		mRightHandCheckComponent->SetBoxSize(5.f, 18.f);
		mRightHandCheckComponent->SetRelativePos(20.f, 9.f);
		mRightHandCheckComponent->SetCollisionProfile("EnvSensor");
		mRightHandCheckComponent->SetCollisionBeginFunction([this](const FVector3& HitPos, CCollider* Other) {
			mMovementComponent->GetSharedState().AddTouchedWall(ETouchedWall::Type::Right, Other);
			});
		mRightHandCheckComponent->SetCollisionEndFunction([this](CCollider* Other) {
			mMovementComponent->GetSharedState().RemoveTouchedWall(ETouchedWall::Type::Right, Other);
			});

#ifdef _DEBUG

		mRightHandCheckComponent->SetDebugDraw(true);

#endif // _DEBUG
	}

	mRightWallCheckComponent = CreateComponent<CColliderBox2D>("RightWallCheck").lock();
	if (mRightWallCheckComponent != nullptr)
	{
		mRightWallCheckComponent->SetBoxSize(5.f, 7.f);
		mRightWallCheckComponent->SetRelativePos(20.f, -10.f);
		mRightWallCheckComponent->SetCollisionProfile("EnvSensor");

#ifdef _DEBUG

		mRightWallCheckComponent->SetDebugDraw(true);

#endif // _DEBUG
	}

	mMoveTestBoxComponent = CreateComponent<CColliderBox2D>("MoveCheck").lock();
	if (mMoveTestBoxComponent != nullptr)
	{
		std::shared_ptr<CColliderBox2D> RootBoxComp = std::static_pointer_cast<CColliderBox2D>(mRoot.lock());
		if (RootBoxComp != nullptr)
		{
			mMoveTestBoxComponent->SetBoxSize(RootBoxComp->GetInfo().HalfSize * 1.5);
		}
		mMoveTestBoxComponent->SetCollisionProfile("EnvSensor");
		mMoveTestBoxComponent->SetSimulatePhysics(true);

#ifdef _DEBUG

		mMoveTestBoxComponent->SetDebugDraw(true);

#endif // _DEBUG
	}
}

void CPlayer::InitCharacterMovements()
{
	if (mMovementComponent != nullptr)
	{
		{
			std::shared_ptr<FPlayerGameState> GameState = std::make_shared<FPlayerGameState>(mSelf);
			GameState->mOnLand.push_back([this, WorldWeakPtr = mWorld]() {
				int32 Idx = std::rand() % 5;
				WorldWeakPtr.lock()->GetWorldAssetManager().lock()->SoundPlay("S_PlayerLand" + std::to_string(Idx));
				mMovementComponent->GetSharedState<FPlayerGameState>().ActiveLazyItems(ELazyItemObject::LandActive);
				});
			mMovementComponent->SetSharedState(GameState);
		}

		/* 걷기 */
		mMovementComponent->AddMovement(std::make_shared<CWalkMovement>());
		/* 벽 미끄러짐 */
		mMovementComponent->AddMovement(std::make_shared<CSlideMovement>());
		/* 점프 */
		mMovementComponent->AddMovement(std::make_shared<CPlayerJumpMovement>());
		/* 벽 점프 */
		{
			std::shared_ptr<CWallJumpMovement> Movement = std::make_shared<CWallJumpMovement>();
			Movement->mOnStartWallJump.push_back([this](const FVector3& Dir, bool NeedToReverseSight) {
				if (NeedToReverseSight == true)
				{
					mAnimStateMachine->ForcedTransition(
						"WallJump",
						FAnimSequenceOptionOverride{
							1.f,
							0.6,
							false,
							false,
							mMovementComponent->GetSharedState<FPlayerGameState>().GetFixedSymmetry(),
							false
						}
					);
				}
				else
				{
					mAnimStateMachine->ForcedTransition("WallJump");
				}
				});
			mMovementComponent->AddMovement(Movement);
		}
		/* 대시 */
		{
			std::shared_ptr<CDashMovement> Movement = std::make_shared<CDashMovement>();
			Movement->mOnSpendPoint.push_back([this](int32 CurrentPoint, int32 ChangedPoint) {
				mHairMeshComp->BlendHairColor(CHairMeshComponent::mStaticHairColors[CurrentPoint]);
				});
			Movement->mOnRechargePoint.push_back([this](int32 CurrentPoint, int32 ChangedPoint) {
				mHairMeshComp->RollBackHairColor(CHairMeshComponent::mStaticHairColors[CurrentPoint]);
				});
			mDashHandle = mMovementComponent->AddMovement(Movement);
		}
		/* 매달리기 */
		{
			std::shared_ptr<CDangleMovement> Movement = std::make_shared<CDangleMovement>();
			Movement->mOnMove.push_back([this]() {
				mBreathAnimation2DComponent->ChangeAnimation("A_BreathDangle");
				});
			Movement->mOnWarnLimit.push_back([this](float CurDanagleTime) {
				mBreathAnimation2DComponent->ChangeAnimation("A_BreathWarn");
				mIsMeshWarn = true;
				mWarnFlickerAccTime = 0.f;
				mWarningIndex = 0;
				});
			Movement->mOnFinish.push_back([this]() {
				mBreathAnimation2DComponent->ChangeAnimation("A_BreathIdle");
				});
			Movement->mOnRechargePoint.push_back([this](float) {
				mIsMeshWarn = false;
				});
			mDangleHandle = mMovementComponent->AddMovement(Movement);
		}
		/* 오르기 */
		mMovementComponent->AddMovement(std::make_shared<CClimbMovement>());
		/* 빠르게 내려가기 */
		mMovementComponent->AddMovement(std::make_shared<CFastFallMovement>());
		/* 죽기 */
		{
			std::shared_ptr<CDeadMovement> Movement = std::make_shared<CDeadMovement>();
			Movement->mOnDead.push_back([this]() {
				mMovementComponent->GetSharedState<FPlayerGameState>().ClearLazyItems();
				PlayDeadCutscene([WorldWeakPtr = mWorld]() {
					auto TileMainWorld = std::static_pointer_cast<CTileMainWorldBase>(WorldWeakPtr.lock());
					if (TileMainWorld != nullptr)
					{
						TileMainWorld->GetTileMapFactory().lock()->RespawnTargetObject();
					}
					});
				});
			Movement->mOnRevive.push_back([this]() {
				PlayRespawnCutscene();
				});
			mMovementComponent->AddMovement(Movement);
		}
		/* 대화하기 */
		mMovementComponent->AddMovement(std::make_shared<CDialogueMovement>());
	}
}

void CPlayer::MoveUp()
{
	mMovementComponent->AddSight(FVector3::Axis[EAxis::Y]);

	FVector3 MoveValue = mRoot.lock()->GetAxis(EAxis::Y);
	mMovementComponent->DoMovement("Climb", &MoveValue);
}

void CPlayer::MoveDown()
{
	mMovementComponent->AddSight(-FVector3::Axis[EAxis::Y]);

	FVector3 MoveValue = -mRoot.lock()->GetAxis(EAxis::Y);
	mMovementComponent->DoMovement("Climb", &MoveValue);
}

void CPlayer::MoveLeft()
{
	mMovementComponent->AddSight(-FVector3::Axis[EAxis::X]);

	FVector3 MoveValue = -mRoot.lock()->GetAxis(EAxis::X);
	mMovementComponent->DoMovement("Walk", &MoveValue);
	mMovementComponent->DoMovement("Slide");
}

void CPlayer::MoveRight()
{
	mMovementComponent->AddSight(FVector3::Axis[EAxis::X]);

	FVector3 MoveValue = mRoot.lock()->GetAxis(EAxis::X);
	mMovementComponent->DoMovement("Walk", &MoveValue);
	mMovementComponent->DoMovement("Slide");
}

void CPlayer::MoveFastFall()
{
	mMovementComponent->DoMovement("FastFall");
}

void CPlayer::MoveSlowFall()
{
	mMovementComponent->StopMovement("FastFall");
}

void CPlayer::MoveJump()
{
	mMovementComponent->DoMovement("Jump");
}

void CPlayer::MoveDrop()
{
	mMovementComponent->StopMovement("Jump");
}

void CPlayer::MoveDash()
{
	mMovementComponent->DoMovement("Dash");
}

void CPlayer::MoveDangle()
{
	mMovementComponent->DoMovement("Dangle");
}

void CPlayer::TryToMoveDangle()
{
	if (mMovementComponent->IsActiveMovement("Jump") == false)
	{
		mMovementComponent->DoMovement("Dangle");
	}
}

void CPlayer::MoveRelease()
{
	mMovementComponent->StopMovement("Dangle");
}

FAnimStateData CPlayer::ApplyPlayerAnim(const std::string& Name, float PlayTime, float PlayRate, bool IsLoop, bool IsReverse, bool IsSymmetry, bool IsSymmetryOnSight)
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

void CPlayer::PlayRespawnCutscene(std::function<void()> Callback)
{
	mScreenSwapWidget->mOnFinishScreenAnim = Callback;
	mScreenSwapWidget->Open();
}

void CPlayer::PlayDeadCutscene(std::function<void()> Callback)
{
	mScreenSwapWidget->mOnFinishScreenAnim = Callback;
	mScreenSwapWidget->Close();
}

#ifdef _DEBUG

void CPlayer::ChangeCamera()
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CCameraManager> CameraMgr = World->GetCameraManager().lock();
	if (mIsDebugCamera == true)
	{
		CameraMgr->ChangeMainCamera(mCameraComponent);
	}
	else
	{
		CameraMgr->ChangeMainCamera(mDebugCameraComponent);
	}
	mIsDebugCamera = !mIsDebugCamera;
}

void CPlayer::ChangeCollisionTreeDraw()
{
	std::shared_ptr<CWorld> World = mWorld.lock();
	std::shared_ptr<CWorldCollision> CollisionMgr = World->GetCollision().lock();
	CollisionMgr->ToggleDrawRenderDebugLine();
}

void CPlayer::ChangeCollisionDraw()
{
	CCollider::mAllowDrawDebug = !CCollider::mAllowDrawDebug;
}

void CPlayer::ChangeHairDraw()
{
	mIsDrawHair = !mIsDrawHair;

	mHairMeshComp->SetEnable(mIsDrawHair);
}

#endif // _DEBUG
