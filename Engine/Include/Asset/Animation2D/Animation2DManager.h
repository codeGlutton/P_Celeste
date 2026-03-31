#pragma once

#include "Animation2D.h"
#include "AnimMetaData.h"

class CAnimation2DManager
{
	friend class CAssetManager;

private:
	CAnimation2DManager();

public:
	~CAnimation2DManager();

public:
	bool Init();
	bool CreateAnimation(const std::string& Name);
	std::weak_ptr<CAnimation2D> FindAnimation(
		const std::string& Name);
	bool SetAnimation2DTextureType(const std::string& Name,
		EAnimation2DTextureType Type);
    bool SetTexture(const std::string& AnimationName,
        const std::weak_ptr<class CTexture>& Texture);
    /*bool SetTexture(const std::string& AnimationName,
        const std::string& Name);
    bool SetTexture(const std::string& AnimationName,
        const std::string& Name,
        const TCHAR* FileName,
        const std::string& PathName = "Texture");
    bool SetTextureFullPath(const std::string& AnimationName,
        const std::string& Name,
        const TCHAR* FullPath);
    bool SetTexture(const std::string& AnimationName,
        const std::string& Name,
        const std::vector<const TCHAR*>& FileName,
        const std::string& PathName = "Texture");
    bool SetTextureFullPath(const std::string& AnimationName,
        const std::string& Name,
        const std::vector<const TCHAR*>& FullPath);*/
    bool AddFrame(const std::string& AnimationName,
        const FVector2& Start, const FVector2& Size);
    bool AddFrame(const std::string& AnimationName,
        float StartX, float StartY, float SizeX,
        float SizeY);
    bool AddFrame(const std::string& AnimationName,
        int Count, const FVector2& Start, const FVector2& Size);
    bool AddFrame(const std::string& AnimationName,
        int Count, float StartX, float StartY, float SizeX,
        float SizeY);
    void ReleaseAsset(const std::string& Name);

public:
    template<typename DataType>
    bool LoadMetaData(const std::string& Name, const TCHAR* FileName,
        const std::string& PathName = "Animation");
    template<typename DataType>
    bool LoadMetaDataFullPath(const std::string& Name, const TCHAR* FullPath);
    std::weak_ptr<CAnimMetaData> FindMetaData(const std::string& Name);

private:
	std::unordered_map<std::string, std::shared_ptr<CAnimation2D>>	  mAnimation2DMap;
	std::unordered_map<std::string, std::shared_ptr<CAnimMetaData>>	  mAnimMetaMap;
};

template<typename DataType>
bool CAnimation2DManager::LoadMetaData(const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	std::weak_ptr<CAnimMetaData> Check = FindMetaData(Name);

	// 있을 경우
	if (Check.expired() == false)
	{
		return true;
	}

	CAnimMetaData* Origin = new DataType;
	std::shared_ptr<CAnimMetaData> AnimMeta;
	AnimMeta.reset(Origin);
	AnimMeta->SetName(Name);

	if (AnimMeta->Load(FileName, PathName) == false)
	{
		return false;
	}

	mAnimMetaMap.insert(std::make_pair(Name, AnimMeta));
	return true;
}

template<typename DataType>
bool CAnimation2DManager::LoadMetaDataFullPath(const std::string& Name, const TCHAR* FullPath)
{
	std::weak_ptr<CAnimMetaData> Check = FindMetaData(Name);

	// 있을 경우
	if (Check.expired() == false)
	{
		return true;
	}

	CAnimMetaData* Origin = new DataType;
	std::shared_ptr<CAnimMetaData> AnimMeta;
	AnimMeta.reset(Origin);
	AnimMeta->SetName(Name);

	if (AnimMeta->Load(FullPath) == false)
	{
		return false;
	}

	mAnimMetaMap.insert(std::make_pair(Name, AnimMeta));
	return true;
}