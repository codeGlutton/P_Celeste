#include "Sound.h"
#include "../PathManager.h"

CSound::CSound()
{
    mAssetType = EAssetType::Sound;
}

CSound::~CSound()
{
    for (auto& Channel : mChannels)
    {
        Channel->stop();
    }
    if (mSound)
        mSound->release();
}

bool CSound::LoadSound(FMOD::System* System, 
    FMOD::ChannelGroup* Group, bool Loop, const char* FileName,
    const std::string& PathName)
{
    mSystem = System;
    mGroup = Group;
    mLoop = Loop;

    char    FullPath[MAX_PATH] = {};

    const char* Path = CPathManager::FindPathMultibyte(PathName);

    if (Path)
        strcpy_s(FullPath, Path);

    strcat_s(FullPath, FileName);

    FMOD_MODE   Mode = FMOD_DEFAULT;

    if (Loop)
        Mode = FMOD_LOOP_NORMAL;

    if (mSystem->createSound(FullPath, Mode, nullptr, &mSound) !=
        FMOD_OK)
        return false;

    return true;
}

bool CSound::LoadSound3D(FMOD::System* System, FMOD::ChannelGroup* Group, bool Loop, const char* FileName, const std::string& PathName)
{
    mSystem = System;
    mGroup = Group;
    mLoop = Loop;

    char    FullPath[MAX_PATH] = {};

    const char* Path = CPathManager::FindPathMultibyte(PathName);

    if (Path)
        strcpy_s(FullPath, Path);

    strcat_s(FullPath, FileName);

    FMOD_MODE   Mode = FMOD_DEFAULT;

    if (Loop)
        Mode = FMOD_LOOP_NORMAL;

    if (mSystem->createSound(FullPath, Mode | FMOD_3D, nullptr, &mSound) !=
        FMOD_OK)
        return false;

    mSound->set3DMinMaxDistance(520.0f, 1560.0f);
    return true;
}

bool CSound::IsPlaying() const
{
    UpdateChannels();

    if (mChannels.empty() == true)
    {
        return false;
    }
    return true;
}

void CSound::Play(bool Fade, float FadeTime)
{
    UpdateChannels();

    FMOD::Channel* NewChannel = nullptr;
    mSystem->playSound(mSound, mGroup, false, &NewChannel);
    mChannels.push_back(NewChannel);

    if (Fade == true)
    {
        FadeIn(NewChannel, FadeTime);
    }

    // 사운드 재생이 끝날때 호출될 함수를 지정한다.
    NewChannel->setUserData(new FSoundThreadUserData(shared_from_this()));
    NewChannel->setCallback(CSound::SoundPlayEnd);
}

void CSound::Play3D(const FVector3& Pos, bool Fade, float FadeTime)
{
    UpdateChannels();

    FMOD::Channel* NewChannel = nullptr;
    mSystem->playSound(mSound, mGroup, false, &NewChannel);
    FMOD_VECTOR ChannelPos = { Pos.x, Pos.y, Pos.z };
    FMOD_VECTOR ChannelVel = { 0.f, 0.f, 0.f };
    NewChannel->set3DAttributes(&ChannelPos, &ChannelVel);
    NewChannel->setMode(FMOD_3D_LINEARROLLOFF);
    mChannels.push_back(NewChannel);

    if (Fade == true)
    {
        FadeIn(NewChannel, FadeTime);
    }

    // 사운드 재생이 끝날때 호출될 함수를 지정한다.
    NewChannel->setUserData(new FSoundThreadUserData(shared_from_this()));
    NewChannel->setCallback(CSound::SoundPlayEnd);
}

void CSound::Stop(bool Fade, float FadeTime)
{
    UpdateChannels();

    if (mChannels.empty() == false)
    {
        std::list<FMOD::Channel*> PreChannels = mChannels;
        for (auto& Channel : PreChannels)
        {
            Channel->stop();
            if (Fade == true)
            {
                FadeOut(Channel, FadeTime);
            }
        }
    }
}

void CSound::Pause()
{
    UpdateChannels();

    if (mChannels.empty() == false)
    {
        for (auto& Channel : mChannels)
        {
            bool IsPaused = false;
            Channel->getPaused(&IsPaused);
            if (IsPaused == false)
            {
                Channel->setPaused(true);
            }
        }
    }
}

void CSound::Resume()
{
    UpdateChannels();

    if (mChannels.empty() == false)
    {
        for (auto& Channel : mChannels)
        {
            bool IsPaused = false;
            Channel->getPaused(&IsPaused);
            if (IsPaused == true)
            {
                Channel->setPaused(false);
            }
        }
    }
}

void CSound::FadeIn(FMOD::Channel* Channel, float FadeTime)
{
    unsigned int FadePoints = 0;
    unsigned long long CurClock = 0;
    FMOD_RESULT Result;

    int Rate = 0;
    FMOD_System_GetSoftwareFormat((FMOD_SYSTEM*)mSystem, &Rate, 0, 0);
    const unsigned long long CalFadeTime = unsigned long long(Rate * FadeTime);

    FMOD_System_LockDSP((FMOD_SYSTEM*)mSystem);
    {
        FMOD_Channel_GetDSPClock((FMOD_CHANNEL*)Channel, nullptr, &CurClock);
        Result = FMOD_Channel_GetFadePoints((FMOD_CHANNEL*)Channel, &FadePoints, 0, 0);
        if (FadePoints != 0)
        {
            // 페이드가 이미 존재

            std::array<float, 2> PreSettingVolumes;
            std::array<unsigned long long, 2> PreSettingPoints;
            Result = FMOD_Channel_GetFadePoints((FMOD_CHANNEL*)Channel, &FadePoints, PreSettingPoints.data(), PreSettingVolumes.data());

            // 현재 페이드에 진입되어 있으며, 페이드 아웃 처리중인 경우
            if (CurClock >= PreSettingPoints[0] && CurClock < PreSettingPoints[1] && PreSettingVolumes[0] > PreSettingVolumes[1])
            {
                // 정리
                FMOD_Channel_SetDelay((FMOD_CHANNEL*)Channel, 0, 0, false);
                FMOD_Channel_RemoveFadePoints((FMOD_CHANNEL*)Channel, CurClock, CurClock + CalFadeTime);

                // 보간 처리
                float InterpT = (CurClock - PreSettingPoints[0]) / (PreSettingPoints[1] - PreSettingPoints[0]);
                float CurVolume = PreSettingVolumes[0] + (PreSettingVolumes[1] - PreSettingVolumes[0]) * InterpT;
                FMOD_Channel_AddFadePoint((FMOD_CHANNEL*)Channel, CurClock, CurVolume);
                FMOD_Channel_AddFadePoint((FMOD_CHANNEL*)Channel, CurClock + (CurClock - PreSettingPoints[0]), 1.f);
            }
        }
        else
        {
            // 페이드가 없을 때

            FMOD_Channel_AddFadePoint((FMOD_CHANNEL*)Channel, CurClock, 0.f);
            FMOD_Channel_AddFadePoint((FMOD_CHANNEL*)Channel, CurClock + CalFadeTime, 1.f);
        }
    }
    FMOD_System_UnlockDSP((FMOD_SYSTEM*)mSystem);
}

void CSound::FadeOut(FMOD::Channel* Channel, float FadeTime)
{
    unsigned int FadePoints = 0;
    unsigned long long CurClock = 0;
    FMOD_RESULT Result;

    int Rate = 0;
    FMOD_System_GetSoftwareFormat((FMOD_SYSTEM*)mSystem, &Rate, 0, 0);
    const unsigned long long CalFadeTime = unsigned long long(Rate * FadeTime);

    FMOD_System_LockDSP((FMOD_SYSTEM*)mSystem);
    {
        FMOD_Channel_GetDSPClock((FMOD_CHANNEL*)Channel, nullptr, &CurClock);
        Result = FMOD_Channel_GetFadePoints((FMOD_CHANNEL*)Channel, &FadePoints, 0, 0);
        if (FadePoints != 0)
        {
            // 페이드가 이미 존재

            std::array<float, 2> PreSettingVolumes;
            std::array<unsigned long long, 2> PreSettingPoints;
            Result = FMOD_Channel_GetFadePoints((FMOD_CHANNEL*)Channel, &FadePoints, PreSettingPoints.data(), PreSettingVolumes.data());

            // 현재 페이드에 진입되어 있으며, 페이드 인 처리중인 경우
            if (CurClock >= PreSettingPoints[0] && CurClock < PreSettingPoints[1] && PreSettingVolumes[0] < PreSettingVolumes[1])
            {
                // 정리
                FMOD_Channel_SetDelay((FMOD_CHANNEL*)Channel, 0, 0, false);
                FMOD_Channel_RemoveFadePoints((FMOD_CHANNEL*)Channel, CurClock, CurClock + CalFadeTime);

                // 보간 처리
                float InterpT = (CurClock - PreSettingPoints[0]) / (PreSettingPoints[1] - PreSettingPoints[0]);
                float CurVolume = PreSettingVolumes[0] + (PreSettingVolumes[1] - PreSettingVolumes[0]) * InterpT;
                FMOD_Channel_AddFadePoint((FMOD_CHANNEL*)Channel, CurClock, CurVolume);
                FMOD_Channel_AddFadePoint((FMOD_CHANNEL*)Channel, CurClock + (CurClock - PreSettingPoints[0]), 0.f);
            }
        }
        else
        {
            // 페이드가 없을 때

            FMOD_Channel_AddFadePoint((FMOD_CHANNEL*)Channel, CurClock, 1.f);
            FMOD_Channel_AddFadePoint((FMOD_CHANNEL*)Channel, CurClock + CalFadeTime, 0.f);
        }
    }
    FMOD_System_UnlockDSP((FMOD_SYSTEM*)mSystem);
}

void CSound::UpdateChannels() const
{
    if (mIsDirty.exchange(false) == true)
    {
        for (auto Iter = mChannels.begin(); Iter != mChannels.end();)
        {
            bool IsPlaying = false;
            (*Iter)->isPlaying(&IsPlaying);

            if (IsPlaying == false)
            {
                Iter = mChannels.erase(Iter);
            }
            else
            {
                ++Iter;
            }
        }
    }
}

FMOD_RESULT CSound::SoundPlayEnd(FMOD_CHANNELCONTROL* channelcontrol,
    FMOD_CHANNELCONTROL_TYPE controltype,
    FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype,
    void* commanddata1, void* commanddata2)
{
    FMOD::Channel* EndChannel = (FMOD::Channel*)channelcontrol;
    
    FSoundThreadUserData* SoundUserDataPtr = nullptr;
    EndChannel->getUserData((void**)&SoundUserDataPtr);
    EndChannel->setUserData(nullptr);
    std::shared_ptr<CSound> SoundPtr = SoundUserDataPtr->mSound.lock();
    if (SoundPtr != nullptr)
    {
        SoundPtr->mIsDirty.store(true);
    }

    delete SoundUserDataPtr;

    return FMOD_OK;
}
