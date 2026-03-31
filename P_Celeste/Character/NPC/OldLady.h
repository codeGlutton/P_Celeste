#pragma once

#include "Character/Character.h"

#include "Animation/AnimStateMachine.h"

class CTileMapCameraComponent;
class CCutsceenCameraComponent;

class COldLady : public CCharacter
{
	friend class CWorld;
	friend class CObject;

protected:
	COldLady();
	COldLady(const COldLady& ref) = default;
	COldLady(COldLady&& ref) noexcept = default;

public:
	virtual ~COldLady() override = default;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

private:
	virtual std::shared_ptr<CCollider> MakeRootCollider() override;
	virtual void ConfigStateMachine(std::shared_ptr<CAnimStateMachine>& StateMachine) override;

public:
	virtual bool CanStartDialogue() override;
	virtual std::shared_ptr<FDialoguePanelBrush> GetDialoguePanelBrush() override;
	virtual std::shared_ptr<FDialoguePortraitBrush> GetDialoguePortraitBrush(const std::string& EmotionName) override;
	virtual std::shared_ptr<FDialogueContext> GetDialogueContext() override;

public:
	void LaughForever();
	void StopLaugh();
	void ChangeCamera(const FVector3& WorldPos, float ZoomRate);
	void ChangeCamera(const FVector3& InitPos, float InitZoomRate, const FVector3& WorldPos, float ZoomRate);
	void RollBackCamera(std::weak_ptr<CTileMapCameraComponent> RollbackCamera, float ZoomRate);
	void RollBackCamera(std::function<void()> mCallBack, const FVector3& WorldPos, float ZoomRate);

public:
	void SetDialogueContext(std::shared_ptr<FDialogueContext> Context)
	{
		mDialogueContext = Context;
	}

private:
	void InitDialogueBrushes();
	void InitCharacterMovements();

private:
	FAnimStateData ApplyOldLadyAnim(
		const std::string& Name,
		float PlayTime,
		float PlayRate,
		bool IsLoop,
		bool IsReverse = false,
		bool IsSymmetry = true,
		bool IsSymmetryOnSight = true
	);

private:
	std::shared_ptr<CCutsceenCameraComponent> mCutsceenCameraComponent;

protected:
	std::shared_ptr<FDialogueContext> mDialogueContext;

protected:
	std::shared_ptr<FDialoguePanelBrush> mDialoguePanelBrush;
	std::unordered_map<std::string, std::shared_ptr<FDialoguePortraitBrush>> mDialoguePortraitBrushes;
};

