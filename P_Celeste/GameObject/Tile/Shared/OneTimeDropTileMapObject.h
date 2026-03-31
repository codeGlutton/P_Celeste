#pragma once

#include "GameObject/Tile/Shared/DropTileMapObjectBase.h"

struct FOneTimeDropTileObjectInitInfo : public FDropTileObjectBaseInitInfo
{
	float mLifeTime = 0.5f;
};

class COneTimeDropTileMapObject : public CDropTileMapObjectBase
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public CDropTileMapObjectBase::FMapRuntimeData
	{
	};

protected:
	COneTimeDropTileMapObject() = default;
	COneTimeDropTileMapObject(const COneTimeDropTileMapObject& ref) = default;
	COneTimeDropTileMapObject(COneTimeDropTileMapObject&& ref) noexcept = default;

public:
	virtual ~COneTimeDropTileMapObject() override = default;

public:
	virtual void LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData) override;
	virtual void SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const override;

public:
	virtual void Update(float DeltaTime) override;

public:
	static std::shared_ptr<COneTimeDropTileMapObject> MakeQuikly(std::shared_ptr<CWorld> World, const FOneTimeDropTileObjectInitInfo& InitInfo);

protected:
	static void MakeQuikly(std::shared_ptr<COneTimeDropTileMapObject> DropTileMap, const FOneTimeDropTileObjectInitInfo& InitInfo);

protected:
	float mLifeTime = 2.0f;
	float mAccLifeTime = 0.f;
};

