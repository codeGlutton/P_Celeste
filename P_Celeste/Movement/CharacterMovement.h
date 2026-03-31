#pragma once

#include "Object.h"
#include "Movement/MovementInfo.h"

class CInteractableObject;
class CGameObject;
class CCharacter;
class CSceneComponent;
class CCollider;
class CCharacterMovementComponent;

struct FSharedGameState;
class CCharacterMovement;

/**
 * 움직임 처리 결과
 */
enum class EMovementResult
{
	None,
	Ongoing, // 지속
	Abort, // 중지
	LooseStack // 정상 스택 소비
};

/**
 * 재시작 요청시 규칙
 */
enum class EMovementRecallRule
{
	Ignore, // 무시
	Stacking, // 스택 누적
	Restart // 재시작
};

/**
 * 정지 원인
 */
enum class EMovementStopReason
{
	Manual,
	Lock,
	Self,
};

class CMovementEffector
{
public:
	CMovementEffector(
		std::weak_ptr<CGameObject> Interactor,
		const std::unordered_map<const CCollider*, std::weak_ptr<CInteractableObject>>& DynamicWallMap,
		std::weak_ptr<CCharacterMovement> Movement
	);
	~CMovementEffector();

private:
	std::weak_ptr<CGameObject> mInteractor;
	std::vector<std::weak_ptr<CInteractableObject>> mInteractableObjects;
	std::weak_ptr<CCharacterMovement> mMovement;
};

class CCharacterMovement abstract : public CObject, public std::enable_shared_from_this<CCharacterMovement>
{
public:
	CCharacterMovement(std::string Name = std::string(), EMovementRecallRule RecallRule = EMovementRecallRule::Ignore) :
		mName(Name),
		mRecallRule(RecallRule)
	{
	}

public:
	virtual void Init(std::weak_ptr<CCharacterMovementComponent> Owner)
	{
		mOwnerComponent = Owner;
	}
	void RequestToCanMove(OUT bool& IsCanMove, OUT const std::vector<std::string>*& LooseTags);
	EMovementResult RequestToStartMove(const void* Payload, OUT bool& IsNewMove);
	EMovementResult RequestToUpdateMove(float DeltaTime);
	void RequestToLooseStack(OUT bool& IsFinish);
	void RequestToAbortMove(EMovementStopReason Reason, OUT bool& IsStop);

protected:
	virtual bool CanMove() const
	{
		return true;
	}
	virtual EMovementResult Move(const void* Payload)
	{
		return EMovementResult::LooseStack;
	}
	virtual void ApplyStack()
	{
	}
	virtual EMovementResult Update(float DeltaTime)
	{
		return EMovementResult::Ongoing;
	}
	virtual void LooseStack()
	{
	}
	virtual bool CanStop(EMovementStopReason Reason) const
	{
		return true;
	}
	virtual void Stop()
	{
	}
	virtual void Finish()
	{
	}

public:
	const std::string& GetName() const
	{
		return mName;
	}
	EMovementRecallRule GetRecallRule() const
	{
		return mRecallRule;
	}
	int32 GetStack() const
	{
		return mStack;
	}
	bool IsOngoing() const
	{
		return mIsOngoing;
	}

public:
	std::shared_ptr<CCharacterMovementComponent> GetOwnerComponent() const;
	std::shared_ptr<CSceneComponent> GetUpdateComponent() const;
	std::shared_ptr<CCollider> GetMoveTestComponent() const;
	std::shared_ptr<CCharacter> GetOwnerCharacter() const;
	FSharedGameState* GetSharedGameState() const;

private:
	void OnMove() const;
	void OnFinish() const;

public:
	std::vector<std::function<void()>> mOnMove;
	std::vector<std::function<void()>> mOnFinish;

private:
	std::weak_ptr<CCharacterMovementComponent> mOwnerComponent;

private:
	const std::string mName;
	const EMovementRecallRule mRecallRule = EMovementRecallRule::Ignore;

	/* 실행 시 제한 */
protected:
	std::vector<std::string> mRequiredTagsOnMove;
	std::vector<std::string> mForbiddenTagsOnMove;
	std::vector<std::string> mLooseTagsOnMove;

	/* 실행 도중 시 제한 */
protected:
	std::vector<std::string> mRequiredTagsOnUpdate;
	std::vector<std::string> mForbiddenTagsOnUpdate;
	std::vector<std::string> mLockTagsOnUpdate;

private:
	int32 mStack = 0;
	bool mIsOngoing = false;
};

