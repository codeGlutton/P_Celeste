#pragma once

#include "GameObject/Tile/MapFactoryObject.h"

class CSoliloquyWidget;

class CStage0MapFactoryObject : public CMapFactoryObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CStage0MapFactoryObject();
	CStage0MapFactoryObject(const CStage0MapFactoryObject& ref) = default;
	CStage0MapFactoryObject(CStage0MapFactoryObject&& ref) noexcept = default;

public:
	virtual ~CStage0MapFactoryObject() override = default;

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

protected:
	std::weak_ptr<CSoliloquyWidget> mSoliloquyWidget;
};

