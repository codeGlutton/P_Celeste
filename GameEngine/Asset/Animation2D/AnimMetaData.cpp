#include "AnimMetaData.h"
#include "../PathManager.h"

CAnimMetaData::CAnimMetaData()
{
	mAssetType = EAssetType::AnimationMeta;
}

bool CAnimMetaData::Load(const TCHAR* FileName, const std::string& PathName)
{
	TCHAR FullPath[MAX_PATH] = {};

	lstrcpy(FullPath, CPathManager::FindPath(PathName));
	lstrcat(FullPath, FileName);

	return Load(FullPath);
}

bool CAnimMetaData::Load(const TCHAR* FullPath)
{
	char FullPathMultibyte[MAX_PATH] = {};

	int Length = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, Length, nullptr, nullptr);

	FILE* File = nullptr;
	fopen_s(&File, FullPathMultibyte, "rb");

	if (File == nullptr)
	{
		return false;
	}

	bool Result = Load(File);
	fclose(File);

	return Result;
}

bool CAnimMetaData::Save(const TCHAR* FileName, const std::string& PathName) const
{
	TCHAR FullPath[MAX_PATH] = {};

	lstrcpy(FullPath, CPathManager::FindPath(PathName));
	lstrcat(FullPath, FileName);

	return Save(FullPath);
}

bool CAnimMetaData::Save(const TCHAR* FullPath) const
{
	char FullPathMultibyte[MAX_PATH] = {};

	int Length = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, Length, nullptr, nullptr);

	FILE* File = nullptr;
	fopen_s(&File, FullPathMultibyte, "wb");

	if (File == nullptr)
	{
		return false;
	}

	bool Result = Save(File);
	fclose(File);

	return Result;
}

