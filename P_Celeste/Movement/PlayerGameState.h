#pragma once

#include "Movement/SharedGameState.h"
#include "Dialogue/Conversable.h"
#include "GameObject/Item/LazyItemObject.h"

/**
 * 플레이어 게임 상태 갱신
 */
struct FPlayerGameState : public FSharedGameState
{
public:
	FPlayerGameState(std::weak_ptr<CGameObject> OwnerObject);

public:
	virtual void Update(float DeltaTime) override;

public:
	bool IsFixedSymmetry() const
	{
		return mIsFixedSymmetry;
	}
	bool GetFixedSymmetry() const
	{
		return mFixedSymmetry;
	}

	float GetAirTime() const
	{
		return mAirTime;
	}

public:
	void SetFixSymmetry(bool Symmetry)
	{
		if (mIsFixedSymmetry == false)
		{
			mIsFixedSymmetry = true;
			mFixedSymmetry = Symmetry;
		}
	}
	void ReleaseFixSymmetry()
	{
		mIsFixedSymmetry = false;
	}

public:
	void AddConversableObject(std::weak_ptr<IConversable> ConversableObject)
	{
		mConversableObject = ConversableObject;
	}
	void RemoveConversableObject(std::weak_ptr<IConversable> ConversableObject)
	{
		if (mConversableObject.lock() == ConversableObject.lock())
		{
			mConversableObject.reset();
		}
	}

public:
	const std::weak_ptr<IConversable>& GetConversableObject() const
	{
		return mConversableObject;
	}

public:
	void AddLazyItems(std::weak_ptr<CLazyItemObject> Item);
	void ClearLazyItems();
	void ActiveLazyItems(ELazyItemObject::Type Type);

protected:
	float mAirTime = 0.f;

protected:
	bool mIsFixedSymmetry = false;
	bool mFixedSymmetry = false;

protected:
	std::weak_ptr<IConversable> mConversableObject;

protected:
	std::array<std::vector<std::weak_ptr<CLazyItemObject>>, ELazyItemObject::Count> mFollowLazyItems;
};

