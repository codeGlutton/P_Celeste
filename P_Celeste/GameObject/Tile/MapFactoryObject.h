#pragma once

#include "Object/GameObject.h"
#include "GameObject/Tile/MapInfo.h"

struct FMapInfoBase;
class CStaticTileMapObject;

struct FParallaxScrollingInfo
{
	FVector2 mFactor;
	FVector2 mOriginWorldPos;
	std::weak_ptr<CSceneComponent> mSceneComp;
};

class CMapFactoryObject : public CGameObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CMapFactoryObject() = default;
	CMapFactoryObject(const CMapFactoryObject& ref) = default;
	CMapFactoryObject(CMapFactoryObject&& ref) noexcept = default;

public:
	virtual ~CMapFactoryObject() override = default;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;
	virtual void PostUpdate(float DeltaTime) override;

public:
	virtual void StartOpening();
	virtual void StartEnding();

public:
	void SetTargetObject(std::shared_ptr<CGameObject> Target);
	void RespawnTargetObject() const;

public:
	const std::weak_ptr<FMapInfoBase>& GetCurTileMapInfo() const
	{
		return mCurMapInfo;
	}
	const std::weak_ptr<CGameObject>& GetTargetObject() const
	{
		return mTargetObject;
	}

protected:
	virtual bool InitMapInfos() = 0;
	virtual bool InitBackgrounds() = 0;
	virtual bool InitDynamicObjects() = 0;
	virtual bool InitUI() = 0;

protected:
	bool InitSimpleBackground(
		int32 IndexX,
		int32 IndexY,
		const FVector3& BaseWorldPos,
		const FVector3& WorldScale, 
		const std::string& RenderLayer, 
		const std::string& TextureName, 
		const FVector2& ParallaxScrollRate
	);
	bool InitSimpleBackgrounds(
		int32 StartX,
		int32 StartY,
		int32 CountX,
		int32 CountY,
		const FVector3& BaseWorldPos,
		const FVector3& WorldScale,
		const std::string& RenderLayer,
		const std::string& TextureName,
		const FVector2& ParallaxScrollRate
	);

protected:
	void LoadSerialTextures(
		const std::string& TextureName,
		const std::wstring& FilePathFormat, 
		int32 Count, 
		int32 StartOffset = 0
	);
	void LoadSerialAnimation2D(
		const int32 FileOffset,
		const int32 FileCount,
		const std::string& AnimName,
		const TCHAR* AnimFileNameFormat,
		const FVector2& FrameSize
	);
	void LoadSerialSounds(
		const std::string& TextureName,
		const std::string& GroupName,
		bool IsLoop,
		const std::string& FilePathFormat,
		int32 Count,
		int32 StartOffset = 0
	);
	void LoadSerialSound3Ds(
		const std::string& TextureName,
		const std::string& GroupName,
		bool IsLoop,
		const std::string& FilePathFormat,
		int32 Count,
		int32 StartOffset = 0
	);

public:
	std::vector<std::function<void()>> mOnChangeMap;

protected:
	std::vector<std::shared_ptr<FMapInfoBase>> mMapInfos;
	std::weak_ptr<FMapInfoBase> mCurMapInfo;
	EMapMoveDirection::Type mLastMoveDir = EMapMoveDirection::Left;

protected:
	std::weak_ptr<CGameObject> mTargetObject;

protected:
	FVector3 mParallZeroPos;
	std::vector<FParallaxScrollingInfo> mBackgroundComps;
	static constexpr float mParallaxSmoothRate = 1.f;
};

