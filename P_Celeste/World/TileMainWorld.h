#pragma once

#include "World/MainWorld.h"
#include "GameObject/Tile/MapFactoryObject.h"
#include "Character/Player/Player.h"

class CTileMainWorldBase : public CMainWorld
{
public:
	CTileMainWorldBase() = default;
	virtual ~CTileMainWorldBase() override = default;

public:
	const std::weak_ptr<CMapFactoryObject>& GetTileMapFactory() const
	{
		return mTileMapFactory;
	}

	const std::weak_ptr<FMapInfoBase>& GetCurTileMapInfo() const
	{
		std::shared_ptr<CMapFactoryObject> Factory = mTileMapFactory.lock();
		return Factory->GetCurTileMapInfo();
	}
	const std::weak_ptr<CGameObject>& GetTargetObject() const
	{
		std::shared_ptr<CMapFactoryObject> Factory = mTileMapFactory.lock();
		return Factory->GetTargetObject();
	}

protected:
	std::weak_ptr<CMapFactoryObject> mTileMapFactory;
};

template<typename TileMapFactoryType, typename PlayerType = CPlayer>
class CTileMainWorld : public CTileMainWorldBase
{
public:
	CTileMainWorld() = default;
	virtual ~CTileMainWorld() override = default;

public:
	virtual bool Init() override
	{
		CMainWorld::Init();

		std::shared_ptr<CMapFactoryObject> TileMapFactory = CreateGameObject<TileMapFactoryType>("TileMapFactory").lock();
		mTileMapFactory = TileMapFactory;

		std::shared_ptr<PlayerType> Player = CreateGameObject<PlayerType>("Player").lock();
		TileMapFactory->SetTargetObject(Player);

		TileMapFactory->StartOpening();

		return true;
	}
};

