#pragma once

#include "World/World.h"

class CEditorPlayer;

class CEditorWorld : public CWorld
{
public:
	CEditorWorld();
	virtual ~CEditorWorld() override;

public:
	virtual bool Init() override;

private:
	void LoadFont();
	void LoadAnimation2D();
	void LoadSound();
	void CreateUI();

private:
	std::weak_ptr<CEditorPlayer> mPlayer;
};
