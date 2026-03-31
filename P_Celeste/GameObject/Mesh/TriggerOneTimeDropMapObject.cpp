#include "Pch.h"
#include "TriggerOneTimeDropMapObject.h"

#include "Component/ColliderBox2D.h"

bool CTriggerOneTimeDropMapObject::Init()
{
	COneTimeDropMapObject::Init();

	mTriggerBox = CreateComponent<CColliderBox2D>("Trigger").lock();

	mTriggerBox->SetCollisionProfile("Item");
#ifdef _DEBUG

	mTriggerBox->SetDebugDraw(true);

#endif // DEBUG
	mTriggerBox->SetInheritScale(false);
	mTriggerBox->SetCollisionBeginFunction(this, &CTriggerOneTimeDropMapObject::OnStartTriggerOverlap);
	mTriggerBox->SetGravitySacle(mDropGravityScale);

	return true;
}

void CTriggerOneTimeDropMapObject::Update(float DeltaTime)
{
	COneTimeDropMapObject::Update(DeltaTime);

	if (mDropState == EDropState::Idle && mIsTrigger == true)
	{
		mAccTriggerTime += DeltaTime;
		if (mAccTriggerTime >= mTriggerDelayTime)
		{
			StartDrop();
		}
	}
}

void CTriggerOneTimeDropMapObject::LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData)
{
	COneTimeDropMapObject::LoadMapRuntimeData(RuntimeData);

	auto& CurRuntimeData = static_cast<CTriggerOneTimeDropMapObject::FMapRuntimeData&>(RuntimeData);
	if (CurRuntimeData.mIsDropBlock == true)
	{
		mIsTrigger = true;
	}
	else
	{
		mIsTrigger = false;
	}
}

void CTriggerOneTimeDropMapObject::SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const
{
	COneTimeDropMapObject::SaveMapRuntimeData(RuntimeData);

	auto& CurRuntimeData = static_cast<CTriggerOneTimeDropMapObject::FMapRuntimeData&>(RuntimeData);
	CurRuntimeData.mIsDropBlock = (mDropState != EDropState::Idle) || mIsTrigger == true;
}

void CTriggerOneTimeDropMapObject::OnStartTriggerOverlap(const FVector3& HitPoint, CCollider* Collision)
{
	if (mDropState == EDropState::Idle && mIsTrigger == false)
	{
		mIsTrigger = true;
		mAccTriggerTime = 0.f;
	}
}

void CTriggerOneTimeDropMapObject::OnLanded(std::shared_ptr<CGameObject> Interator)
{
	if (mOnlyDropByTrigger == true)
	{
		CDynamicMapObject::OnLanded(Interator);
	}
	else
	{
		COneTimeDropMapObject::OnLanded(Interator);
	}
}

void CTriggerOneTimeDropMapObject::OnGraped(std::shared_ptr<CGameObject> Interator)
{
	if (mOnlyDropByTrigger == true)
	{
		CDynamicMapObject::OnGraped(Interator);
	}
	else
	{
		COneTimeDropMapObject::OnGraped(Interator);
	}
}

void CTriggerOneTimeDropMapObject::StartDrop()
{
	COneTimeDropMapObject::StartDrop();
	mIsTrigger = true;
}

std::shared_ptr<CTriggerOneTimeDropMapObject> CTriggerOneTimeDropMapObject::MakeQuikly(std::shared_ptr<CWorld> World, const FTriggerOneTimeDropObjectInitInfo& InitInfo)
{
	std::shared_ptr<CTriggerOneTimeDropMapObject> TriggerTileMap = CDynamicMapObject::MakeQuikly<CTriggerOneTimeDropMapObject>(World, InitInfo);
	MakeQuikly(TriggerTileMap, InitInfo);

	return TriggerTileMap;
}

void CTriggerOneTimeDropMapObject::MakeQuikly(std::shared_ptr<CTriggerOneTimeDropMapObject> TirggerMapObject, const FTriggerOneTimeDropObjectInitInfo& InitInfo)
{
	COneTimeDropMapObject::MakeQuikly(TirggerMapObject, InitInfo);

	TirggerMapObject->mTriggerBox->SetWorldPos(InitInfo.mTriggerBoxWorldPos + InitInfo.mTriggerBoxSize / 2.f);
	TirggerMapObject->mTriggerBox->SetBoxSize(InitInfo.mTriggerBoxSize);

	TirggerMapObject->mOnlyDropByTrigger = InitInfo.mOnlyDropByTrigger;
	TirggerMapObject->mTriggerDelayTime = InitInfo.mTriggerDelayTime;
}

