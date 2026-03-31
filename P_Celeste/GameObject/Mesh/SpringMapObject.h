#pragma once

#include "GameObject/Mesh/DynamicMapObject.h"
#include "GameObject/Tile/MapInfo.h"

class CCharacter;
class CAnimation2DComponent;

enum class ESpringState : uint8
{
	Idle,
	Trigger,
	Reload,
};

struct FSpringObjectInitInfo : public FDynamicObjectInitInfo
{
	std::string mAnimationName;
	FVector2 mCollisionSize;
	ETouchedWall::Type mSpringDir = ETouchedWall::Type::Up;
	float mSpringVelocity = 200.f;
};

class CSpringMapObject : public CDynamicMapObject
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public FMapRuntimeDataBase
	{
	};

protected:
	CSpringMapObject() = default;
	CSpringMapObject(const CSpringMapObject& ref) = default;
	CSpringMapObject(CSpringMapObject&& ref) noexcept = default;

public:
	virtual ~CSpringMapObject() override = default;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

protected:
	virtual std::shared_ptr<CCollider> MakeMainCollider() override;

public:
	virtual void OnLanded(std::shared_ptr<CGameObject> Interator) override;
	virtual void OnJumped(std::shared_ptr<CGameObject> Interator) override;
	virtual void OnGraped(std::shared_ptr<CGameObject> Interator) override;
	virtual void OnReleased(std::shared_ptr<CGameObject> Interator) override;
	virtual void OnTouchedDown(ETouchedWall::Type Type, std::shared_ptr<CGameObject> Interator) override;

protected:
	virtual void TriggerSpring(std::shared_ptr<CCharacter> Interator);
	bool CanTrigger(ETouchedWall::Type Type) const;

public:
	static std::shared_ptr<CSpringMapObject> MakeQuikly(std::shared_ptr<CWorld> World, const FSpringObjectInitInfo& InitInfo);

protected:
	static void MakeQuikly(std::shared_ptr<CSpringMapObject> SpringMap, const FSpringObjectInitInfo& InitInfo);

protected:
	std::shared_ptr<CAnimation2DComponent> mAnimation2DComponent;

protected:
	ESpringState mSpringState = ESpringState::Idle;
	ETouchedWall::Type mSpringDir = ETouchedWall::Type::Up;

protected:
	float mSpringVelocity = 200.f;
	float mTriggerDelayTime = 0.1f;
	float mTriggerDelayAccTime = mTriggerDelayTime;

private:
	std::weak_ptr<CCharacter> mTriggeredInterator;
};
