#pragma once

#include "../Asset.h"
#include "fmod.hpp"

#include <atomic>

#pragma comment(lib, "fmod_vc.lib")

struct FSoundThreadUserData
{
public:
    FSoundThreadUserData(std::shared_ptr<class CSound> Sound) :
        mSound(Sound)
    {
    }

    std::weak_ptr<class CSound> mSound;
};

class CSound :
    public CAsset, public std::enable_shared_from_this<CSound>
{
    friend class CSoundManager;

protected:
    CSound();

public:
    virtual ~CSound();

public:
    bool LoadSound(FMOD::System* System, FMOD::ChannelGroup* Group,
        bool Loop, const char* FileName,
        const std::string& PathName = "Sound");
    bool LoadSound3D(FMOD::System* System, FMOD::ChannelGroup* Group,
        bool Loop, const char* FileName,
        const std::string& PathName = "Sound");
    bool IsPlaying() const;
    void Play(bool Fade = false, float FadeTime = 0.5f);
    void Play3D(const FVector3& Pos, bool Fade = false, float FadeTime = 0.5f);
    void Stop(bool Fade = false, float FadeTime = 0.5f);
    void Pause();
    void Resume();

private:
    void FadeIn(FMOD::Channel* Channel, float FadeTime);
    void FadeOut(FMOD::Channel* Channel, float FadeTime);
    void UpdateChannels() const;

public:
    static FMOD_RESULT SoundPlayEnd(
        FMOD_CHANNELCONTROL* channelcontrol, 
        FMOD_CHANNELCONTROL_TYPE controltype, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
        void* commanddata1, void* commanddata2);

protected:
    FMOD::System* mSystem = nullptr;
    FMOD::ChannelGroup* mGroup = nullptr;
    mutable std::list<FMOD::Channel*> mChannels;
    FMOD::Sound* mSound = nullptr;

    bool        mLoop = false;

private:
    mutable std::atomic<bool> mIsDirty = false;
};

