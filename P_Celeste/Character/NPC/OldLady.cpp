#include "Pch.h"
#include "OldLady.h"

#include "Component/MeshComponent.h"
#include "Component/Animation2DComponent.h"
#include "Component/ColliderBox2D.h"
#include "Component/CharacterMovementComponent.h"
#include "Component/WidgetComponent.h"
#include "Component/CutsceenCameraComponent.h"
#include "Component/TileMapCameraComponent.h"

#include "Device.h"
#include "World/World.h"
#include "World/CameraManager.h"
#include "World/TileMainWorld.h"

#include "Movement/SharedGameState.h"
#include "Movement/NPC/LaughMovement.h"

COldLady::COldLady()
{
	SetClassType<COldLady>();
}

bool COldLady::Init()
{
	CCharacter::Init();

	InitCharacterMovements();
	InitDialogueBrushes();

	if (mMovementComponent != nullptr)
	{
		mMovementComponent->SetSpeed(320.f);
		mMovementComponent->SetAccel(1500.f);
	}

	if (mMeshComponent != nullptr)
	{
		mMeshComponent->SetRelativePos(0.f, 30.f);
		mMeshComponent->SetRenderLayer("BackObject");
	}

	mCutsceenCameraComponent = CreateComponent<CCutsceenCameraComponent>("CutsceenCamera").lock();
	if (mCutsceenCameraComponent != nullptr)
	{
		const FResolution& RS = CDevice::GetInst()->GetResolution();
		mCutsceenCameraComponent->SetProjection(ECameraProjectionType::Ortho, 90.f, RS.Width, RS.Height, 1000.f);
		mCutsceenCameraComponent->SetInheritScale(false);
		mCutsceenCameraComponent->SetInheritRot(false);
	}

	return true;
}

void COldLady::Update(float DeltaTime)
{
	CCharacter::Update(DeltaTime);

	auto TileWorld = std::static_pointer_cast<CTileMainWorldBase>(mWorld.lock());
	FVector3 TargetWorldPos = TileWorld->GetTargetObject().lock()->GetWorldPos();

	FVector3 SightDir = TargetWorldPos - GetWorldPos();
	SightDir.Normalize();
	mMovementComponent->SetSight(SightDir);
}

std::shared_ptr<CCollider> COldLady::MakeRootCollider()
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

void COldLady::ConfigStateMachine(std::shared_ptr<CAnimStateMachine>& StateMachine)
{
	if (mAnimation2DComponent != nullptr)
	{
		mAnimation2DComponent->AddAnimation("A_OldLadyIdle");
		mAnimation2DComponent->AddAnimation("A_OldLadyLaugh");
	}

	{
		StateMachine->CreateState("Idle", [this]() {
			return ApplyOldLadyAnim("A_OldLadyIdle", 1.f, 1.f, true);
			}, true);

		StateMachine->CreateState("Laugh", [this]() {
			return ApplyOldLadyAnim("A_OldLadyLaugh", 1.f, 1.f, true);
			});
	}

	/* To Idle */
	{
		auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
			std::shared_ptr<COldLady> OldLady = std::static_pointer_cast<COldLady>(Comp->GetOwner().lock());
			if (OldLady != nullptr)
			{
				return OldLady->mMovementComponent->IsActiveMovement("Laugh") == false;
			}
			return false;
			};

		StateMachine->AddTransition("Laugh", "Idle", Lamda);
	}

	/* To Laugh */
	{
		auto Lamda = [](std::shared_ptr<const CAnimation2DComponent> Comp, float DeltaTime) {
			std::shared_ptr<COldLady> OldLady = std::static_pointer_cast<COldLady>(Comp->GetOwner().lock());
			if (OldLady != nullptr)
			{
				return OldLady->mMovementComponent->IsActiveMovement("Laugh") == true;
			}
			return false;
			};

		StateMachine->AddTransition("Idle", "Laugh", Lamda);
	}
}

bool COldLady::CanStartDialogue()
{
	return true;
}

std::shared_ptr<FDialoguePanelBrush> COldLady::GetDialoguePanelBrush()
{
	return mDialoguePanelBrush;
}

std::shared_ptr<FDialoguePortraitBrush> COldLady::GetDialoguePortraitBrush(const std::string& EmotionName)
{
	if (mDialoguePortraitBrushes.find(EmotionName) == mDialoguePortraitBrushes.end())
	{
		return nullptr;
	}
	return mDialoguePortraitBrushes.at(EmotionName);
}

std::shared_ptr<FDialogueContext> COldLady::GetDialogueContext()
{
	return mDialogueContext;
}

void COldLady::InitDialogueBrushes()
{
	mDialoguePanelBrush = std::make_shared<FDialoguePanelBrush>();
	{
		mDialoguePanelBrush->mPanelTexture = "T_OldLadyPanel";
		mDialoguePanelBrush->mTint = FVector4::White;
	}

	mDialoguePortraitBrushes.insert({ "Normal", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["Normal"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_OldLadyPortraitNormal", 5, 4);
		PortraitBrush->mSounds = GetEmotionSoundNames("S_OldLadyPortraitNormal", 10, 3);
		PortraitBrush->mPerSound = { "S_OldLadyPortraitPerNormal", 10 };
		PortraitBrush->mIsLoop = false;
	}
	mDialoguePortraitBrushes.insert({ "NormalLoop", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["NormalLoop"];

		std::vector<std::string> TextureLhs = GetEmotionTextureNames("T_OldLadyPortraitNormal", 4, 0);
		std::vector<std::string> TextureRhs = GetEmotionTextureNames("T_OldLadyPortraitNormal", 6, 9);
		PortraitBrush->mTextures.reserve(TextureLhs.size() + TextureRhs.size());
		PortraitBrush->mTextures = std::move(TextureLhs);
		PortraitBrush->mTextures.insert(PortraitBrush->mTextures.end(), std::make_move_iterator(TextureRhs.begin()), std::make_move_iterator(TextureRhs.end()));
		PortraitBrush->mIsLoop = true;
		PortraitBrush->mLoopDelayFrame = 45;
	}
	mDialoguePortraitBrushes.insert({ "LaughLoop", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["LaughLoop"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_OldLadyPortraitLaugh", 4, 0);
		PortraitBrush->mIsLoop = true;
		PortraitBrush->mPlayRate = 0.5f;
	}
	mDialoguePortraitBrushes.insert({ "CreepA", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["CreepA"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_OldLadyPortraitCreepA", 5, 0);
		PortraitBrush->mSounds = GetEmotionSoundNames("S_OldLadyPortraitMonk", 10, 3);
		PortraitBrush->mPerSound = { "S_OldLadyPortraitPerMonk", 10 };
		PortraitBrush->mIsLoop = false;
	}
	mDialoguePortraitBrushes.insert({ "CreepALoop", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["CreepALoop"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_OldLadyPortraitCreepAIdle", 1, 0);
		PortraitBrush->mIsLoop = true;
		PortraitBrush->mLoopDelayFrame = 45;
	}
	mDialoguePortraitBrushes.insert({ "CreepB", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["CreepB"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_OldLadyPortraitCreepB", 5, 0);
		PortraitBrush->mSounds = GetEmotionSoundNames("S_OldLadyPortraitMonk", 10, 3);
		PortraitBrush->mPerSound = { "S_OldLadyPortraitPerMonk", 10 };
		PortraitBrush->mIsLoop = false;
	}
	mDialoguePortraitBrushes.insert({ "CreepBLoop", std::make_shared<FDialoguePortraitBrush>() });
	{
		std::shared_ptr<FDialoguePortraitBrush>& PortraitBrush = mDialoguePortraitBrushes["CreepBLoop"];

		PortraitBrush->mTextures = GetEmotionTextureNames("T_OldLadyPortraitCreepBIdle", 1, 0);
		PortraitBrush->mIsLoop = true;
		PortraitBrush->mLoopDelayFrame = 45;
	}
}

void COldLady::InitCharacterMovements()
{
	if (mMovementComponent != nullptr)
	{
		mMovementComponent->SetSharedState(std::make_shared<FSharedGameState>(mSelf));

		/* 웃기 */
		{
			mMovementComponent->AddMovement(std::make_shared<CLaughMovement>());
		}
	}
}

void COldLady::LaughForever()
{
	mMovementComponent->DoMovement("Laugh");
}

void COldLady::StopLaugh()
{
	mMovementComponent->StopMovement("Laugh");
}

void COldLady::ChangeCamera(const FVector3& WorldPos, float ZoomRate)
{
	mCutsceenCameraComponent->SetMoveRelativePos(WorldPos - GetWorldPos());
	mCutsceenCameraComponent->SetTargetZoomRate(ZoomRate);

	mWorld.lock()->GetCameraManager().lock()->ChangeMainCamera(mCutsceenCameraComponent);
}

void COldLady::ChangeCamera(const FVector3& InitPos, float InitZoomRate, const FVector3& WorldPos, float ZoomRate)
{
	mCutsceenCameraComponent->SetWorldPos(InitPos);
	mCutsceenCameraComponent->SetZoomRate(InitZoomRate);

	mCutsceenCameraComponent->SetMoveRelativePos(WorldPos - GetWorldPos());
	mCutsceenCameraComponent->SetTargetZoomRate(ZoomRate);

	mWorld.lock()->GetCameraManager().lock()->ChangeMainCamera(mCutsceenCameraComponent);
}

void COldLady::RollBackCamera(std::weak_ptr<CTileMapCameraComponent> RollbackCamera, float ZoomRate)
{
	mCutsceenCameraComponent->SetMoveRelativePos(RollbackCamera.lock()->GetClampCameraPos() - GetWorldPos());
	mCutsceenCameraComponent->SetTargetZoomRate(ZoomRate);
	mCutsceenCameraComponent->OnEndCutsceen = [RollbackCamera]() {
		std::shared_ptr<CCameraComponent> CameraComp = RollbackCamera.lock();
		CameraComp->GetWorld().lock()->GetCameraManager().lock()->ChangeMainCamera(CameraComp);
		};
}

void COldLady::RollBackCamera(std::function<void()> mCallBack, const FVector3& WorldPos, float ZoomRate)
{
	mCutsceenCameraComponent->SetMoveRelativePos(WorldPos - GetWorldPos());
	mCutsceenCameraComponent->SetTargetZoomRate(ZoomRate);
	mCutsceenCameraComponent->OnEndCutsceen = mCallBack;
}

FAnimStateData COldLady::ApplyOldLadyAnim(const std::string& Name, float PlayTime, float PlayRate, bool IsLoop, bool IsReverse, bool IsSymmetry, bool IsSymmetryOnSight)
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