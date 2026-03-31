#pragma once

#include "World/World.h"

class CMainWorld : public CWorld
{
public:
	CMainWorld();
	virtual ~CMainWorld() override;

public:
	virtual bool Init() override;

private:
	void LoadAnimation2D();
	void LoadFont();
	void LoadSound();
	void CreateUI();

private:
	void LoadPlayerAnimation2D(
		const int32 FileOffset,
		const int32 FileCount,
		const std::string& AnimName,
		const TCHAR* AnimFileNameFormat
	);
	void LoadPlayerAnimation2D(
		const int32 FileOffset,
		const int32 FileCount,
		const std::string& AnimName,
		const TCHAR* AnimFileNameFormat,
		const TCHAR* MetaFileName
	);

protected:
	void LoadSerialTextures(
		const std::string& TextureName,
		const std::wstring& FilePathFormat, 
		int32 Count, 
		int32 StartOffset = 0
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
};

