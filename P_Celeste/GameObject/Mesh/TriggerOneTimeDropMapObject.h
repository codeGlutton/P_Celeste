#pragma once

#include "GameObject/Mesh/OneTimeDropMapObject.h"

class CColliderBox2D;

struct FTriggerOneTimeDropObjectInitInfo : public FOneTimeDropObjectInitInfo
{
	FVector3 mTriggerBoxWorldPos;
	FVector2 mTriggerBoxSize;
	bool mOnlyDropByTrigger = false;
	float mTriggerDelayTime = 0.f;
};

class CTriggerOneTimeDropMapObject : public COneTimeDropMapObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CTriggerOneTimeDropMapObject() = default;
	CTriggerOneTimeDropMapObject(const CTriggerOneTimeDropMapObject& ref) = default;
	CTriggerOneTimeDropMapObject(CTriggerOneTimeDropMapObject&& ref) noexcept = default;

public:
	virtual ~CTriggerOneTimeDropMapObject() override = default;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

public:
	virtual void LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData) override;
	virtual void SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const override;

private:
	virtual void OnStartTriggerOverlap(const FVector3& HitPoint, CCollider* Collision);

public:
	virtual void OnLanded(std::shared_ptr<CGameObject> Interator) override;
	virtual void OnGraped(std::shared_ptr<CGameObject> Interator) override;

protected:
	virtual void StartDrop() override;

public:
	static std::shared_ptr<CTriggerOneTimeDropMapObject> MakeQuikly(std::shared_ptr<CWorld> World, const FTriggerOneTimeDropObjectInitInfo& InitInfo);
	static void MakeQuikly(std::shared_ptr<CTriggerOneTimeDropMapObject> TirggerMapObject, const FTriggerOneTimeDropObjectInitInfo& InitInfo);

protected:
	std::shared_ptr<CColliderBox2D> mTriggerBox;
	bool mOnlyDropByTrigger = false;
	float mTriggerDelayTime = 0.f;

protected:
	bool mIsTrigger = false;
	float mAccTriggerTime = 0.f;
};

