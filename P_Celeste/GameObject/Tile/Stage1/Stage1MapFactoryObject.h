#pragma once

#include "GameObject/Tile/MapFactoryObject.h"

class CStage1MapFactoryObject : public CMapFactoryObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CStage1MapFactoryObject();
	CStage1MapFactoryObject(const CStage1MapFactoryObject& ref) = default;
	CStage1MapFactoryObject(CStage1MapFactoryObject&& ref) noexcept = default;

public:
	virtual ~CStage1MapFactoryObject() override = default;

protected:
	virtual bool InitMapInfos() override;
	virtual bool InitBackgrounds() override;
	virtual bool InitDynamicObjects() override;
	virtual bool InitUI() override;

private:
	void InitMapAssets();

public:
	virtual void StartOpening() override;
	virtual void StartEnding() override;
};

