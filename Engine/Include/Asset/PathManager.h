#pragma once

#include "../EngineInfo.h"

class CPathManager
{
public:
	static void Clear();
	static bool Init();
	static bool CreatePath(const std::string& Name, const TCHAR* Path,
		const std::string& BasePathName = "Root");
	static const TCHAR* FindPath(const std::string& Name);
	static const char* FindPathMultibyte(const std::string& Name);

private:
	static std::unordered_map<std::string, TCHAR*>	mPathMap;
};

