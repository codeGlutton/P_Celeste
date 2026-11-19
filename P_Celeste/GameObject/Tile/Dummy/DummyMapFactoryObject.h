#pragma once

#include "GameObject/Tile/MapFactoryObject.h"

class CDummyMapFactoryObject : public CMapFactoryObject
{
	friend class CWorld;
	friend class CObject;

public:
	// 벤치마크용 더미 구역 총 개수 (초기값 변경으로 구역 수 조정 가능)
	static constexpr int32 mDummyZoneCount = 25;

protected:
	CDummyMapFactoryObject();
	CDummyMapFactoryObject(const CDummyMapFactoryObject& ref) = default;
	CDummyMapFactoryObject(CDummyMapFactoryObject&& ref) noexcept = default;

public:
	virtual ~CDummyMapFactoryObject() override = default;

protected:
	virtual bool InitMapInfos() override;
	virtual bool InitBackgrounds() override;
	virtual bool InitDynamicObjects() override;
	virtual bool InitUI() override;

private:
	void InitMapAssets();
};
