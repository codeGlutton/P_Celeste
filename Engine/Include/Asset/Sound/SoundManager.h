#pragma once

#include "Sound.h"

class CSoundManager
{
	friend class CAssetManager;

private:
	CSoundManager();

public:
	~CSoundManager();

public:
	bool Init();
	void Update();
	bool CreateChannelGroup(const std::string& Name);
	bool CreateChannelGroup(const std::string& Name, const std::string& ParentName);
	bool LoadSound(const std::string& Name,
		const std::string& GroupName, bool Loop, const char* FileName,
		const std::string& PathName = "Sound");
	bool LoadSound3D(const std::string& Name,
		const std::string& GroupName, bool Loop, const char* FileName,
		const std::string& PathName = "Sound");
	std::weak_ptr<CSound> FindSound(const std::string& Name);

	bool IsPlaying(const std::string& Name) const;
	bool IsPlayingChannelGroup(const std::string& Name) const;
	void Play(const std::string& Name, bool Fade = false, float FadeTime = 0.5f);
	void Play3D(const std::string& Name, const FVector3& Pos, bool Fade = false, float FadeTime = 0.5f);
	void Stop(const std::string& Name, bool Fade = false, float FadeTime = 0.5f);
	void Pause(const std::string& Name);
	void Resume(const std::string& Name);
	// Volume : 0 ~ 100
	void SetMasterVolume(int Volume);
	void SetGroupVolume(const std::string & GroupName, int Volume);

private:
	FMOD::ChannelGroup* FindChannelGroup(const std::string& Name);

public:
	void ReleaseAsset(const std::string& Name);

private:
	std::unordered_map<std::string, std::shared_ptr<CSound>>	mSoundMap;
	std::unordered_map<std::string, FMOD::ChannelGroup*>	mChannelGroupMap;
	FMOD::System* mSystem = nullptr;
	FMOD::ChannelGroup* mMasterGroup = nullptr;
};

