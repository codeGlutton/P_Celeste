#pragma once

#include "GameObject/Mesh/DropMapObjectBase.h"

struct FOneTimeDropObjectInitInfo : public FDropObjectBaseInitInfo
{
	float mLifeTime = 0.5f;
};

class COneTimeDropMapObject : public CDropMapObjectBase
{
	friend class CWorld;
	friend class CObject;

public:
	struct FMapRuntimeData : public CDropMapObjectBase::FMapRuntimeData
	{
	};

protected:
	COneTimeDropMapObject() = default;
	COneTimeDropMapObject(const COneTimeDropMapObject& ref) = default;
	COneTimeDropMapObject(COneTimeDropMapObject&& ref) noexcept = default;

public:
	virtual ~COneTimeDropMapObject() override = default;

public:
	virtual void LoadMapRuntimeData(FMapRuntimeDataBase& RuntimeData) override;
	virtual void SaveMapRuntimeData(FMapRuntimeDataBase& RuntimeData) const override;

public:
	virtual void Update(float DeltaTime) override;

public:
	static std::shared_ptr<COneTimeDropMapObject> MakeQuikly(std::shared_ptr<CWorld> World, const FOneTimeDropObjectInitInfo& InitInfo);

protected:
	static void MakeQuikly(std::shared_ptr<COneTimeDropMapObject> DropMap, const FOneTimeDropObjectInitInfo& InitInfo);

protected:
	float mLifeTime = 2.0f;
	float mAccLifeTime = 0.f;
};

