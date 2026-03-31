#pragma once

#include "World/World.h"

class CTitleWidget;

class CTitleWorld : public CWorld
{
public:
	CTitleWorld();
	virtual ~CTitleWorld() override;

public:
	virtual bool Init() override;
	virtual void ClearWorld() override;

private:
	void LoadFont();
	void LoadAnimation2D();
	void LoadSound();
	void CreateUI();

protected:
	std::weak_ptr<CTitleWidget>	mTitleWidget;
};
