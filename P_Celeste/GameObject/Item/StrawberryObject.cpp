#include "Pch.h"
#include "StrawberryObject.h"

#include "Component/SceneComponent.h"
#include "Component/ColliderBox2D.h"
#include "Component/MeshComponent.h"
#include "Component/Animation2DComponent.h"
#include "Component/SpringArmComponent.h"

#include "Object/RewardManager.h"
#include "GameObject/Item/CelesteRewardData.h"

#include "Character/Player/Player.h"

bool CStrawberryObject::Init()
{
    CLazyItemObject::Init();

    mAnimation2DComponent = CreateComponent<CAnimation2DComponent>("Animation2D").lock();
    if (mAnimation2DComponent != nullptr)
    {
        mAnimation2DComponent->SetUpdateComponent(mMeshComponent);
        mAnimation2DComponent->AddAnimation("A_StrawberryIdle", 1.f, 1.f, true);
        mAnimation2DComponent->AddAnimation("A_StrawberryGet");
        mAnimation2DComponent->AddAnimation("A_GhostberryIdle", 1.f, 1.f, true);
        mAnimation2DComponent->AddAnimation("A_GhostberryGet");
    }

    if (mSpringArmComponent != nullptr)
    {
        mSpringArmComponent->SetMaxDist(250.f);
    }

    return true;
}

void CStrawberryObject::Update(float DeltaTime)
{
    CLazyItemObject::Update(DeltaTime);

    if (GetAttachedParent().expired() == true)
    {
        mMeshComponent->SetRelativePos(0.f, std::cos(mShakeAccRadian) * mMeshComponent->GetWorldScale().y * 0.1f);
        mShakeAccRadian += (DeltaTime * mShakeSpeed);
        mShakeAccRadian = std::fmodf(mShakeAccRadian, DirectX::XM_2PI);
    }
}

std::shared_ptr<CCollider> CStrawberryObject::MakeMainCollider()
{
    std::shared_ptr<CColliderBox2D> BerryBoxComp = CreateComponent<CColliderBox2D>("Berry", "SpringArm").lock();

#ifdef _DEBUG

    BerryBoxComp->SetDebugDraw(true);

#endif // DEBUG
    BerryBoxComp->SetInheritScale(false);

    return BerryBoxComp;
}

void CStrawberryObject::OnFollowItem(std::shared_ptr<CPlayer> Player)
{
    mSpawnStrawberryPos = GetWorldPos();
    CLazyItemObject::OnFollowItem(Player);

    if (mIsGhost == false)
    {
        mWorld.lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_StrawberryTouch", GetWorldPos());
    }
    else
    {
        mWorld.lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_GhostberryTouch", GetWorldPos());
    }
}

void CStrawberryObject::OnReleaseItem(std::shared_ptr<CPlayer> Player)
{
    CLazyItemObject::OnReleaseItem(Player);
    SetWorldPos(mSpawnStrawberryPos);
}

void CStrawberryObject::OnActiveItem(std::shared_ptr<CPlayer> Player)
{
    CLazyItemObject::OnActiveItem(Player);

    SaveFromRewardFile();
    Player->DetachGameObject(mSelf.lock());

    if (mIsGhost == false)
    {
        mAnimation2DComponent->ChangeAnimation("A_StrawberryGet");
        mAnimation2DComponent->SetFinishNotify("A_StrawberryGet", [this]() {
            Destroy();
            });

        mWorld.lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_StrawberryGet", GetWorldPos());
    }
    else
    {
        mAnimation2DComponent->ChangeAnimation("A_GhostberryGet");
        mAnimation2DComponent->SetFinishNotify("A_GhostberryGet", [this]() {
            Destroy();
            });

        mWorld.lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_GhostberryGet", GetWorldPos());
    }
}

void CStrawberryObject::LoadFromRewardFile()
{
    std::shared_ptr<CCelesteRewardData> RewardData = CRewardManager::GetInst()->GetData<CCelesteRewardData>();
    if (RewardData != nullptr)
    {
        auto& Collection = RewardData->mStrawberryCollection;
        if (Collection.find(mStrawberryIndex) != Collection.end())
        {
            mIsGhost = true;
        }
        else
        {
            mIsGhost = false;
        }
    }
}

void CStrawberryObject::SaveFromRewardFile()
{
    std::shared_ptr<CCelesteRewardData> RewardData = CRewardManager::GetInst()->GetData<CCelesteRewardData>();
    if (RewardData != nullptr)
    {
        auto& Collection = RewardData->mStrawberryCollection;
        Collection.insert(mStrawberryIndex);
    }
}

std::shared_ptr<CStrawberryObject> CStrawberryObject::MakeQuikly(std::shared_ptr<CWorld> World, const FStrawberryInitInfo& InitInfo)
{
    std::shared_ptr<CStrawberryObject> BerryObj = CItemObject::MakeQuikly<CStrawberryObject>(World, InitInfo);
    MakeQuikly(BerryObj, InitInfo);

    return BerryObj;
}

void CStrawberryObject::MakeQuikly(std::shared_ptr<CStrawberryObject> ItemObj, const FStrawberryInitInfo& InitInfo)
{
	CLazyItemObject::MakeQuikly(ItemObj, InitInfo);

	std::shared_ptr<CColliderBox2D> BoxCollisionComp = std::static_pointer_cast<CColliderBox2D>(ItemObj->mBodyComponent);
	if (BoxCollisionComp != nullptr)
	{
		BoxCollisionComp->SetRelativePos(InitInfo.mCollisionSize / 2.f);
		BoxCollisionComp->SetBoxSize(InitInfo.mCollisionSize);
	}

    std::shared_ptr<CSpringArmComponent>& SpringComp = ItemObj->mSpringArmComponent;
    if (SpringComp != nullptr)
    {
        ItemObj->mSpringArmComponent->SetDesiredRelativePos(FVector3::Zero);
    }

    ItemObj->mStrawberryIndex = InitInfo.mStrawberryIndex;

    ItemObj->LoadFromRewardFile();
    std::shared_ptr<CAnimation2DComponent>& AnimComp = ItemObj->mAnimation2DComponent;
    if (AnimComp != nullptr)
    {
        AnimComp->ChangeAnimation((ItemObj->mIsGhost == false) ? "A_StrawberryIdle" : "A_GhostberryIdle");
    }
}
