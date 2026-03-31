#include "SoundManager.h"
#include "../../World/WorldManager.h"
#include "../../World/CameraManager.h"

CSoundManager::CSoundManager()
{
}

CSoundManager::~CSoundManager()
{
	mSoundMap.clear();

	auto	iter = mChannelGroupMap.begin();
	auto	iterEnd = mChannelGroupMap.end();

	for (; iter != iterEnd; ++iter)
	{
		iter->second->release();
	}

	if (mSystem)
	{
		mSystem->release();
		mSystem->close();
	}
}

bool CSoundManager::Init()
{
	FMOD_RESULT result = FMOD::System_Create(&mSystem);

	if (result != FMOD_OK)
		return false;

	result = mSystem->init(256, FMOD_INIT_NORMAL, nullptr);

	if (result != FMOD_OK)
		return false;

	result = mSystem->set3DSettings(1.0f, 1.0f, 1.0f);

	if (result != FMOD_OK)
		return false;

	result = mSystem->getMasterChannelGroup(&mMasterGroup);

	if (result != FMOD_OK)
		return false;

	mChannelGroupMap.insert(std::make_pair("Master", mMasterGroup));

	{
		CreateChannelGroup("World");

		CreateChannelGroup("BGM", "World");
		CreateChannelGroup("SFX", "World");
		CreateChannelGroup("UI", "World");

		SetGroupVolume("SFX", 30);
		SetGroupVolume("UI", 90);
	}

	{
		CreateChannelGroup("PopUp");

		CreateChannelGroup("PopUpBGM", "PopUp");
		CreateChannelGroup("PopUpSFX", "PopUp");
		CreateChannelGroup("PopUpUI", "PopUp");

		SetGroupVolume("PopUpSFX", 30);
		SetGroupVolume("PopUpUI", 90);
	}

	return true;
}

void CSoundManager::Update()
{
	std::shared_ptr<CWorld> CurWorld = CWorldManager::GetInst()->GetWorld().lock();
	if (CurWorld != nullptr)
	{
		FVector3 CameraPos = CurWorld->GetCameraManager().lock()->GetMainCameraWorldPos();
		FVector3 CameraForward = CurWorld->GetCameraManager().lock()->GetMainCameraAxis(EAxis::Z);
		FVector3 CameraUp = CurWorld->GetCameraManager().lock()->GetMainCameraAxis(EAxis::Y);

		FMOD_VECTOR ListenerPos = { CameraPos.x, CameraPos.y, CameraPos.z };
		FMOD_VECTOR ListenerVel = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR Forward = { CameraForward.x, CameraForward.y, CameraForward.z };
		FMOD_VECTOR Up = { CameraUp.x, CameraUp.y, CameraUp.z };

		mSystem->set3DListenerAttributes(0, &ListenerPos, &ListenerVel, &Forward, &Up);
	}

	mSystem->update();
}

bool CSoundManager::CreateChannelGroup(const std::string& Name)
{
	FMOD::ChannelGroup* Group = FindChannelGroup(Name);

	if (Group)
		return true;

	FMOD_RESULT	result = mSystem->createChannelGroup(Name.c_str(),
		&Group);

	if (result != FMOD_OK)
		return false;

	// 생성된 그룹을 마스터 그룹에 추가한다.
	mMasterGroup->addGroup(Group, false);

	mChannelGroupMap.insert(std::make_pair(Name, Group));

	return true;
}

bool CSoundManager::CreateChannelGroup(const std::string& Name, const std::string& ParentName)
{
	FMOD::ChannelGroup* Group = FindChannelGroup(Name);
	if (Group != nullptr)
	{
		return true;
	}

	FMOD::ChannelGroup* ParentGroup = FindChannelGroup(ParentName);
	if (ParentGroup == nullptr)
	{
		ParentGroup = mMasterGroup;
	}

	FMOD_RESULT	result = mSystem->createChannelGroup(Name.c_str(), &Group);

	if (result != FMOD_OK)
		return false;

	ParentGroup->addGroup(Group, false);

	mChannelGroupMap.insert(std::make_pair(Name, Group));

	return true;
}

bool CSoundManager::LoadSound(const std::string& Name,
	const std::string& GroupName, bool Loop, const char* FileName,
	const std::string& PathName)
{
	if (!FindSound(Name).expired())
		return true;

	FMOD::ChannelGroup* Group = FindChannelGroup(GroupName);

	if (!Group)
		Group = mMasterGroup;

	std::shared_ptr<CSound>	Sound;
	Sound.reset(new CSound);

	Sound->SetName(Name);

	if (!Sound->LoadSound(mSystem, Group, Loop, FileName, PathName))
		return false;

	mSoundMap.insert(std::make_pair(Name, Sound));

	return true;
}

bool CSoundManager::LoadSound3D(const std::string& Name, const std::string& GroupName, bool Loop, const char* FileName, const std::string& PathName)
{
	if (!FindSound(Name).expired())
		return true;

	FMOD::ChannelGroup* Group = FindChannelGroup(GroupName);

	if (!Group)
		Group = mMasterGroup;

	std::shared_ptr<CSound>	Sound;
	Sound.reset(new CSound);

	Sound->SetName(Name);

	if (!Sound->LoadSound3D(mSystem, Group, Loop, FileName, PathName))
		return false;

	mSoundMap.insert(std::make_pair(Name, Sound));

	return true;
}

std::weak_ptr<CSound> CSoundManager::FindSound(const std::string& Name)
{
	auto	iter = mSoundMap.find(Name);

	if (iter == mSoundMap.end())
		return std::weak_ptr<CSound>();

	return iter->second;
}

bool CSoundManager::IsPlaying(const std::string& Name) const
{
	auto	iter = mSoundMap.find(Name);

	if (iter == mSoundMap.end())
		return false;

	return iter->second->IsPlaying();
}

bool CSoundManager::IsPlayingChannelGroup(const std::string& Name) const
{
	if (mChannelGroupMap.find(Name) == mChannelGroupMap.end())
	{
		return false;
	}

	bool IsPlaying = false;
	FMOD_RESULT Result = mChannelGroupMap.at(Name)->isPlaying(&IsPlaying);
	if (Result != FMOD_OK)
	{
		return false;
	}

	return IsPlaying;
}

void CSoundManager::Play(const std::string& Name, bool Fade, float FadeTime)
{
	auto	iter = mSoundMap.find(Name);

	if (iter == mSoundMap.end())
		return;

	iter->second->Play(Fade, FadeTime);
}

void CSoundManager::Play3D(const std::string& Name, const FVector3& Pos, bool Fade, float FadeTime)
{
	auto	iter = mSoundMap.find(Name);

	if (iter == mSoundMap.end())
		return;

	iter->second->Play3D(Pos, Fade, FadeTime);
}

void CSoundManager::Stop(const std::string& Name, bool Fade, float FadeTime)
{
	auto	iter = mSoundMap.find(Name);

	if (iter == mSoundMap.end())
		return;

	iter->second->Stop(Fade, FadeTime);
}

void CSoundManager::Pause(const std::string& Name)
{
	auto	iter = mSoundMap.find(Name);

	if (iter == mSoundMap.end())
		return;

	iter->second->Pause();
}

void CSoundManager::Resume(const std::string& Name)
{
	auto	iter = mSoundMap.find(Name);

	if (iter == mSoundMap.end())
		return;

	iter->second->Resume();
}

void CSoundManager::SetMasterVolume(int Volume)
{
	if (Volume < 0)
		Volume = 0;

	else if (Volume > 100)
		Volume = 100;

	mMasterGroup->setVolume(Volume / 100.f);
}

void CSoundManager::SetGroupVolume(const std::string& GroupName,
	int Volume)
{
	FMOD::ChannelGroup* Group = FindChannelGroup(GroupName);

	if (!Group)
		return;

	if (Volume < 0)
		Volume = 0;

	else if (Volume > 100)
		Volume = 100;

	Group->setVolume(Volume / 100.f);
}

FMOD::ChannelGroup* CSoundManager::FindChannelGroup(
	const std::string& Name)
{
	auto	iter = mChannelGroupMap.find(Name);

	if (iter == mChannelGroupMap.end())
		return nullptr;

	return iter->second;
}

void CSoundManager::ReleaseAsset(const std::string& Name)
{
	auto	iter = mSoundMap.find(Name);

	if (iter != mSoundMap.end())
	{
		// 다른 월드에서 더이상 들고 있지 않을 경우
		if (iter->second.use_count() == 1)
		{
			mSoundMap.erase(iter);
		}
	}
}
