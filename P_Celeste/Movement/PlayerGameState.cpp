#include "Pch.h"
#include "PlayerGameState.h"

#include "Object/GameObject.h"
#include "Character/Player/Player.h"

#include "GameObject/Item/LazyItemObject.h"

FPlayerGameState::FPlayerGameState(std::weak_ptr<CGameObject> OwnerObject) :
	FSharedGameState(OwnerObject)
{
	mOnLand.push_back([this]() {
		mAirTime = 0.f;
		});
}

void FPlayerGameState::Update(float DeltaTime)
{
	FSharedGameState::Update(DeltaTime);
	
	if (IsLandingOnWall() == false)
	{
		mAirTime += DeltaTime;
	}
}

void FPlayerGameState::AddLazyItems(std::weak_ptr<CLazyItemObject> Item)
{
	std::shared_ptr<CLazyItemObject> ItemSharedPtr = Item.lock();
	std::shared_ptr<CPlayer> OwnerSharedPtr = std::static_pointer_cast<CPlayer>(mOwnerObject.lock());

	mFollowLazyItems[ItemSharedPtr->GetLazyType()].push_back(std::move(Item));
	OwnerSharedPtr->AttachGameObject(ItemSharedPtr);
	ItemSharedPtr->OnFollowItem(OwnerSharedPtr);
}

void FPlayerGameState::ClearLazyItems()
{
	std::shared_ptr<CPlayer> OwnerSharedPtr = std::static_pointer_cast<CPlayer>(mOwnerObject.lock());
	for (auto& Items : mFollowLazyItems)
	{
		for (auto& Item : Items)
		{
			std::shared_ptr<CLazyItemObject> ItemSharedPtr = Item.lock();
			OwnerSharedPtr->DetachGameObject(ItemSharedPtr);
			ItemSharedPtr->OnReleaseItem(OwnerSharedPtr);
		}
		Items.clear();
	}
}

void FPlayerGameState::ActiveLazyItems(ELazyItemObject::Type Type)
{
	std::shared_ptr<CPlayer> OwnerSharedPtr = std::static_pointer_cast<CPlayer>(mOwnerObject.lock());
	for (auto& Item : mFollowLazyItems[Type])
	{
		std::shared_ptr<CLazyItemObject> ItemSharedPtr = Item.lock();
		ItemSharedPtr->OnActiveItem(OwnerSharedPtr);
	}
	mFollowLazyItems[Type].clear();
}
