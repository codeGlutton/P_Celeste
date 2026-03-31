#include "RewardManager.h"

#include "../Asset/PathManager.h"

CRewardManager* CRewardManager::mInst = nullptr;

void CRewardManager::Save() const
{
	TCHAR FullPath[MAX_PATH] = {};
	lstrcpy(FullPath, CPathManager::FindPath("Save"));
	lstrcat(FullPath, TEXT("Reward.sav"));

	char FullPathMultibyte[MAX_PATH] = {};
	int Length = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, Length, nullptr, nullptr);

	FILE* File = nullptr;
	fopen_s(&File, FullPathMultibyte, "wb");
	if (File == nullptr)
	{
		return;
	}

	mRewardData->Save(File);

	fclose(File);
}

void CRewardManager::Load()
{
	TCHAR FullPath[MAX_PATH] = {};
	lstrcpy(FullPath, CPathManager::FindPath("Save"));
	lstrcat(FullPath, TEXT("Reward.sav"));

	char FullPathMultibyte[MAX_PATH] = {};
	int Length = WideCharToMultiByte(CP_ACP, 0, FullPath, -1, nullptr, 0, nullptr, nullptr);
	WideCharToMultiByte(CP_ACP, 0, FullPath, -1, FullPathMultibyte, Length, nullptr, nullptr);

	FILE* File = nullptr;
	fopen_s(&File, FullPathMultibyte, "rb");
	if (File == nullptr)
	{
		return;
	}

	mRewardData->Load(File);

	fclose(File);
}

