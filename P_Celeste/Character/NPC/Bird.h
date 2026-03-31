#pragma once

#include "Character/Character.h"

#include "Animation/AnimStateMachine.h"

class CWidgetComponent;

class CBird : public CCharacter
{
	friend class CWorld;
	friend class CObject;

protected:
	CBird();
	CBird(const CBird& ref) = default;
	CBird(CBird&& ref) noexcept = default;

public:
	virtual ~CBird() override = default;

public:
	virtual bool Init() override;

private:
	virtual std::shared_ptr<CCollider> MakeRootCollider() override;
	virtual void ConfigStateMachine(std::shared_ptr<CAnimStateMachine>& StateMachine) override;

public:
	void Cry();
	void Fly(const FVector3& MovePos);
	void Fly(const FVector3& MovePos, std::function<void()> Callback);

private:
	void OnFlyEnd();

public:
	void StartTalk(const std::wstring& MainText, const std::wstring& SubText, const std::string TextureName);
	void StopTalk();

private:
	void InitCharacterMovements();

private:
	FAnimStateData ApplyBirdAnim(
		const std::string& Name,
		float PlayTime,
		float PlayRate,
		bool IsLoop,
		bool IsReverse = false,
		bool IsSymmetry = true,
		bool IsSymmetryOnSight = true
	);

protected:
	std::shared_ptr<CWidgetComponent> mWidgetComponent;

private:
	float mWaitAnimTime = 4.f;
	float mTalkWidgetOffsetY = 10.f;

private:
	std::function<void()> mOnFlyEndCallback;
};

