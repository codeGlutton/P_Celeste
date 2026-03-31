#pragma once

#include "World/World.h"

class CWorldLoadThread;

class CLoadWorld : public CWorld
{
public:
	CLoadWorld();
	virtual ~CLoadWorld() override;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;

public:
	void Load(EWorldType WorldType);

private:
	void LoadAnimation2D();
	void LoadSound();
	void CreateUI();

private:
	std::shared_ptr<CWorldLoadThread> mThread = nullptr;
	EWorldType mLoadType = EWorldType::None;
};
