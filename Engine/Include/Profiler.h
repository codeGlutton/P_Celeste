#pragma once

#include "EngineInfo.h"

enum class EProfileSection : unsigned char
{
	WorldUpdate = 0,
	WorldPostUpdate,
	Collision,
	Render,
	MoveMap,
	Count
};

struct FProfilerSnapshot
{
	float	CurrentFPS = 0.f;
	float	AvgFPS = 0.f;
	float	OnePercentLowFPS = 0.f;
	float	CurrentFrameMs = 0.f;
	float	AvgFrameMs = 0.f;
	float	MaxFrameMs = 0.f;

	double	WorldUpdateMs = 0.0;
	double	WorldPostUpdateMs = 0.0;
	double	CollisionMs = 0.0;
	double	RenderMs = 0.0;
	double	LastMoveMapMs = 0.0;
	double	MaxMoveMapMs = 0.0;

	size_t	ObjectCount = 0;
	size_t	ColliderCount = 0;
	size_t	RenderCompCount = 0;

	int		ActiveZones = 0;
	int		TotalZones = 0;
	bool	StreamingEnabled = true;

	double	PrivateMemMB = 0.0;
	double	WorkingSetMB = 0.0;

	bool	IsRecording = false;
	size_t	RecordedFrames = 0;
	std::string	LastSavedCsvPath;
};

#ifdef _DEBUG

struct FFrameProfileSample
{
	unsigned long long	FrameIndex = 0;
	double	FrameTimeMs = 0.0;
	double	FPS = 0.0;
	double	WorldUpdateMs = 0.0;
	double	WorldPostUpdateMs = 0.0;
	double	CollisionMs = 0.0;
	double	RenderMs = 0.0;
	double	MoveMapMs = 0.0;
	size_t	ObjectCount = 0;
	size_t	ColliderCount = 0;
	size_t	RenderCompCount = 0;
	int		ActiveZones = 0;
	int		TotalZones = 0;
	bool	StreamingEnabled = true;
	double	PrivateMemMB = 0.0;
	double	WorkingSetMB = 0.0;
};

class CProfiler
{
public:
	static void Init();
	static void BeginFrame();
	static void EndFrame();

	static void AddSectionTimeMs(EProfileSection Section, double ElapsedMs);
	static void SetEntityCounters(size_t ObjectCount, size_t ColliderCount, size_t RenderCompCount);
	static void SetZoneInfo(int ActiveZones, int TotalZones, bool StreamingEnabled);

	static bool ToggleRecording();
	static bool StartRecording();
	static std::string StopAndSaveCsv();
	static bool IsRecording();

	static FProfilerSnapshot GetSnapshot();
	static LARGE_INTEGER Now();
	static double ElapsedMs(const LARGE_INTEGER& Start, const LARGE_INTEGER& End);

private:
	static void QueryProcessMemoryMB(double& OutPrivateMB, double& OutWorkingSetMB);

private:
	static constexpr size_t	mRollingWindowSize = 120;

	static LARGE_INTEGER	mFrequency;
	static LARGE_INTEGER	mFrameStart;
	static bool				mInitialized;
	static unsigned long long	mFrameIndex;

	static double	mSectionMs[static_cast<size_t>(EProfileSection::Count)];
	static double	mLastSectionMs[static_cast<size_t>(EProfileSection::Count)];
	static double	mLastMoveMapMs;
	static double	mMaxMoveMapMs;

	static size_t	mObjectCount;
	static size_t	mColliderCount;
	static size_t	mRenderCompCount;

	static int		mActiveZones;
	static int		mTotalZones;
	static bool		mStreamingEnabled;

	static std::vector<double>	mRecentFrameMs;
	static size_t				mRecentWriteIndex;
	static double				mLastFrameMs;
	static double				mPeakFrameMs;

	static bool								mIsRecording;
	static std::vector<FFrameProfileSample>	mRecordedSamples;
	static std::string						mLastSavedCsvPath;
};

class FScopeProfileTimer
{
public:
	explicit FScopeProfileTimer(EProfileSection Section) :
		mSection(Section),
		mStart(CProfiler::Now())
	{
	}

	~FScopeProfileTimer()
	{
		const LARGE_INTEGER End = CProfiler::Now();
		CProfiler::AddSectionTimeMs(mSection, CProfiler::ElapsedMs(mStart, End));
	}

private:
	EProfileSection	mSection;
	LARGE_INTEGER	mStart;
};

#else

class CProfiler
{
public:
	static void Init() {}
	static void BeginFrame() {}
	static void EndFrame() {}
	static void AddSectionTimeMs(EProfileSection, double) {}
	static void SetEntityCounters(size_t, size_t, size_t) {}
	static void SetZoneInfo(int, int, bool) {}
	static bool ToggleRecording() { return false; }
	static bool StartRecording() { return false; }
	static std::string StopAndSaveCsv() { return ""; }
	static bool IsRecording() { return false; }
	static FProfilerSnapshot GetSnapshot() { return FProfilerSnapshot{}; }
};

class FScopeProfileTimer
{
public:
	explicit FScopeProfileTimer(EProfileSection) {}
};

#endif // _DEBUG
