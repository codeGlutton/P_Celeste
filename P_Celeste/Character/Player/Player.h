#pragma once

#include "Character/Character.h"

#include "Animation/AnimStateMachine.h"
#include "Dialogue/DialogueData.h"

#include "Movement/MovementInfo.h"

class CCameraComponent;
class CTileMapCameraComponent;
class CColliderBox2D;
class CHairMeshComponent;

class CScreenSwapWidget;

class CPlayer : public CCharacter
{
	friend class CWorld;
	friend class CObject;

protected:
	CPlayer();
	CPlayer(const CPlayer& ref) = default;
	CPlayer(CPlayer&& ref) noexcept = default;

public:
	virtual ~CPlayer() override = default;

public:
	virtual bool Init() override;
	virtual void Begin() override;
	virtual void Update(float DeltaTime) override;
	virtual void PostUpdate(float DeltaTime) override;
	virtual void Destroy() override;

public:
	virtual float TakeDamage(float Damage) override;
	void ForcedRechageMovements();

private:
	virtual std::shared_ptr<CCollider> MakeRootCollider() override;
	virtual void BindPlayerInputs(std::shared_ptr<CInput>& InputManager) override;
	virtual void ConfigStateMachine(std::shared_ptr<CAnimStateMachine>& StateMachine) override;

public:
	virtual std::shared_ptr<FDialoguePanelBrush> GetDialoguePanelBrush() override;
	virtual std::shared_ptr<FDialoguePortraitBrush> GetDialoguePortraitBrush(const std::string& EmotionName) override;

public:
	std::shared_ptr<CCameraComponent> GetCameraComp();

private:
	void InitDialogueBrushes();
	void InitCheckBoxCollisions();
	void InitCharacterMovements();

private:
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();

private:
	void MoveFastFall();
	void MoveSlowFall();
	void MoveJump();
	void MoveDrop();
	void MoveDash();
	void MoveDangle();
	void TryToMoveDangle();
	void MoveRelease();

private:
	FAnimStateData ApplyPlayerAnim(
		const std::string& Name, 
		float PlayTime, 
		float PlayRate, 
		bool IsLoop, 
		bool IsReverse = false,
		bool IsSymmetry = true,
		bool IsSymmetryOnSight = true
	);

public:
	void PlayRespawnCutscene(std::function<void()> Callback = nullptr);
	void PlayDeadCutscene(std::function<void()> Callback = nullptr);

#ifdef _DEBUG

private:
	void ChangeCamera();

private:
	void ChangeCollisionTreeDraw();

private:
	void ChangeCollisionDraw();

private:
	void ChangeHairDraw();

private:
	std::shared_ptr<CCameraComponent> mDebugCameraComponent;
	bool mIsDebugCamera = false;
	bool mIsDrawHair = true;

#endif // _DEBUG

private:
	std::shared_ptr<CTileMapCameraComponent> mCameraComponent;

private:
	std::shared_ptr<CMeshComponent> mBreathMeshComponent;
	std::shared_ptr<CAnimation2DComponent> mBreathAnimation2DComponent;

	std::shared_ptr<CHairMeshComponent> mHairMeshComp;

private:
	std::shared_ptr<CColliderBox2D> mLeftHandCheckComponent;
	std::shared_ptr<CColliderBox2D> mLeftWallCheckComponent;
	std::shared_ptr<CColliderBox2D> mRightHandCheckComponent;
	std::shared_ptr<CColliderBox2D> mRightWallCheckComponent;
	std::shared_ptr<CColliderBox2D> mUpWallCheckComponent;
	std::shared_ptr<CColliderBox2D> mLeftFootCheckComponent;
	std::shared_ptr<CColliderBox2D> mRightFootCheckComponent;

private:
	std::shared_ptr<CColliderBox2D> mMoveTestBoxComponent;

protected:
	std::shared_ptr<FDialoguePanelBrush> mDialoguePanelBrush;
	std::unordered_map<std::string, std::shared_ptr<FDialoguePortraitBrush>> mDialoguePortraitBrushes;

private:
	std::shared_ptr<CScreenSwapWidget> mScreenSwapWidget;

private:
	bool mIsMeshWarn = false;
	
	std::array<FVector4, 2> mWarningColor = { FVector4::Red, FVector4::White };
	int32 mWarningIndex = 0;

	float mWarnFlickerAccTime = 0.f;
	float mWarnFlickerFrameTime = 0.1f;

private:
	const float mRespawnDelayTime = 2.f;

private:
	FMovementHandle mDashHandle;
	FMovementHandle mDangleHandle;
};

