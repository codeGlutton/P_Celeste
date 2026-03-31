#pragma once

#include "GameObject/Item/LazyItemObject.h"

class Collider;
class CAnimation2DComponent;

struct FStrawberryInitInfo : public FItemObjectInitInfo
{
	FVector2 mCollisionSize;
	int32 mStrawberryIndex;
};

class CStrawberryObject : public CLazyItemObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CStrawberryObject()
	{
		mLazyType = ELazyItemObject::LandActive;
	}
	CStrawberryObject(const CStrawberryObject& ref) = default;
	CStrawberryObject(CStrawberryObject&& ref) noexcept = default;

public:
	virtual ~CStrawberryObject() override = default;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

protected:
	virtual std::shared_ptr<CCollider> MakeMainCollider() override;

public:
	virtual void OnFollowItem(std::shared_ptr<CPlayer> Player) override;
	virtual void OnReleaseItem(std::shared_ptr<CPlayer> Player) override;

public:
	virtual void OnActiveItem(std::shared_ptr<CPlayer> Player) override;

public:
	int32 GetStrawberryIndex() const
	{
		return mStrawberryIndex;
	}

	void LoadFromRewardFile();
	void SaveFromRewardFile();

public:
	static std::shared_ptr<CStrawberryObject> MakeQuikly(std::shared_ptr<CWorld> World, const FStrawberryInitInfo& InitInfo);

protected:
	static void MakeQuikly(std::shared_ptr<CStrawberryObject> ItemObj, const FStrawberryInitInfo& InitInfo);

protected:
	std::shared_ptr<CAnimation2DComponent> mAnimation2DComponent;

protected:
	int32 mStrawberryIndex;
	FVector3 mSpawnStrawberryPos;
	bool mIsGhost;

protected:
	float mShakeSpeed = 3.f;
	float mShakeAccRadian = 0.f;
};

