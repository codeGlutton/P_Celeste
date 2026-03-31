#pragma once

class CTexture;

struct FDialoguePanelBrush
{
public:
	std::string mPanelTexture;
	FVector4 mTint = FVector4::White;
};

struct FDialoguePortraitBrush
{
public:
	void SetRepeatCount(int32 TextCount)
	{
		const int32 OneRepeatTextSize = mTextures.size() * 2;
		mRepeatCount = (TextCount + OneRepeatTextSize - 1) / OneRepeatTextSize;
	}
	void Reset()
	{
		mFrame = 0;
		mAccTime = 0.f;
		mAccDelayFrame = 0;
		mAccRepeatCount = 0;
		mIsEnd = false;
		mIsChangedFrame = true;
	}
	void PlayAnimation(float DeltaTime)
	{
		if (mAnimationEnable == false)
		{
			return;
		}

		int32 mPreFrame = mFrame;
		mAccTime += DeltaTime * mPlayRate;

		while (mAccTime >= mFrameTime)
		{
			mAccTime -= mFrameTime;

			++mFrame;
			if (mFrame == mTextures.size())
			{
				if (mIsLoop == true)
				{
					++mAccDelayFrame;
					if (mAccDelayFrame >= mLoopDelayFrame)
					{
						mAccDelayFrame = 0;
						mFrame = 0;
					}
					else
					{
						mFrame = mTextures.size() - 1;
					}
				}
				else
				{
					++mAccRepeatCount;
					if (mAccRepeatCount >= mRepeatCount)
					{
						mIsEnd = true;
						mFrame = mTextures.size() - 1;
						break;
					}
					else
					{
						mFrame = 0;
					}
				}
			}
		}
		mIsChangedFrame = mPreFrame != mFrame;
	}
	const std::string& GetCurTextureName() const
	{
		return mTextures[mFrame];
	}
	std::string GetCurSoundName() const
	{
		if (mIsChangedFrame == false || mSounds.size() <= mFrame)
		{
			return {};
		}

		if (mIsLoop == false)
		{
			if (mAccRepeatCount == 0 && mFrame == 0)
			{
				return mPerSound.first + std::to_string(std::rand() % mPerSound.second);
			}
			if (mAccRepeatCount == mRepeatCount - 1 && mFrame == static_cast<int32>(mTextures.size()) - 1)
			{
				return mPerSound.first + std::to_string(std::rand() % mPerSound.second);
			}
		}

		int32 SoundIdx = mFrame % mSounds.size();
		return mSounds[SoundIdx].first + std::to_string(std::rand() % mSounds[SoundIdx].second);
	}

public:
	static constexpr float mFrameTime = 1.f / 12.f;
	int32 mFrame = 0;
	float mAccTime = 0.f;
	int32 mAccDelayFrame = 0;
	int32 mAccRepeatCount = 0;

public:
	int32 mRepeatCount = 1;

public:
	bool mAnimationEnable = true;

	std::vector<std::string> mTextures;
	std::vector<std::pair<std::string, int32>> mSounds;
	std::pair<std::string, int32> mPerSound;
	FVector4 mTint = FVector4::White;

	float mPlayRate = 0.8f;
	bool mIsLoop = false;
	int32 mLoopDelayFrame = 1;

public:
	bool mIsChangedFrame = true;
	bool mIsEnd = false;
};

enum class EDialogueTurn
{
	Player,
	Other
};

struct FDialogueData
{
public:
	bool IsNeedChangeAnim(const FDialogueData& PreData) const
	{
		return PreData.mTurn != mTurn || PreData.mLoopEmotionName != mLoopEmotionName || mStartEmotionName.empty() == false;
	}

public:
	EDialogueTurn mTurn;
	std::string mStartEmotionName;
	std::string mLoopEmotionName;
	std::wstring mText;

public:
	std::function<void()> mOnEnd;
};

struct FDialogueContext
{
	std::vector<FDialogueData> mDialogueDatas;
};

