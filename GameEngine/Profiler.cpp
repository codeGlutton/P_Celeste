#include "Profiler.h"
#include "Asset/PathManager.h"

#ifdef _DEBUG

#include <psapi.h>
#pragma comment(lib, "psapi.lib")

LARGE_INTEGER	CProfiler::mFrequency = {};
LARGE_INTEGER	CProfiler::mFrameStart = {};
bool				CProfiler::mInitialized = false;
unsigned long long	CProfiler::mFrameIndex = 0;

double	CProfiler::mSectionMs[static_cast<size_t>(EProfileSection::Count)] = {};
double	CProfiler::mLastSectionMs[static_cast<size_t>(EProfileSection::Count)] = {};
double	CProfiler::mLastMoveMapMs = 0.0;
double	CProfiler::mMaxMoveMapMs = 0.0;

size_t	CProfiler::mObjectCount = 0;
size_t	CProfiler::mColliderCount = 0;
size_t	CProfiler::mRenderCompCount = 0;

int		CProfiler::mActiveZones = 0;
int		CProfiler::mTotalZones = 0;
bool	CProfiler::mStreamingEnabled = true;

std::vector<double>	CProfiler::mRecentFrameMs;
size_t				CProfiler::mRecentWriteIndex = 0;
double				CProfiler::mLastFrameMs = 0.0;
double				CProfiler::mPeakFrameMs = 0.0;

bool								CProfiler::mIsRecording = false;
std::vector<FFrameProfileSample>	CProfiler::mRecordedSamples;
std::string							CProfiler::mLastSavedCsvPath;

void CProfiler::Init()
{
	if (mInitialized)
	{
		return;
	}

	QueryPerformanceFrequency(&mFrequency);
	mRecentFrameMs.reserve(mRollingWindowSize);
	mRecordedSamples.reserve(3600);
	mInitialized = true;
}

void CProfiler::BeginFrame()
{
	if (!mInitialized)
	{
		Init();
	}

	for (size_t i = 0; i < static_cast<size_t>(EProfileSection::Count); ++i)
	{
		mSectionMs[i] = 0.0;
	}

	mFrameStart = Now();
}

void CProfiler::EndFrame()
{
	const LARGE_INTEGER FrameEnd = Now();
	mLastFrameMs = ElapsedMs(mFrameStart, FrameEnd);
	if (mLastFrameMs <= 0.0)
	{
		mLastFrameMs = 0.001;
	}

	if (mLastFrameMs > mPeakFrameMs)
	{
		mPeakFrameMs = mLastFrameMs;
	}

	if (mRecentFrameMs.size() < mRollingWindowSize)
	{
		mRecentFrameMs.push_back(mLastFrameMs);
	}
	else
	{
		mRecentFrameMs[mRecentWriteIndex] = mLastFrameMs;
		mRecentWriteIndex = (mRecentWriteIndex + 1) % mRollingWindowSize;
	}

	const double MoveMapMs = mSectionMs[static_cast<size_t>(EProfileSection::MoveMap)];
	if (MoveMapMs > 0.0)
	{
		mLastMoveMapMs = MoveMapMs;
		if (MoveMapMs > mMaxMoveMapMs)
		{
			mMaxMoveMapMs = MoveMapMs;
		}
	}

	// UI 오버레이가 프레임 도중에 읽더라도 직전 완료 프레임의 일관된 수치를 제공
	for (size_t i = 0; i < static_cast<size_t>(EProfileSection::Count); ++i)
	{
		mLastSectionMs[i] = mSectionMs[i];
	}

	if (mIsRecording)
	{
		FFrameProfileSample Sample;
		Sample.FrameIndex = mFrameIndex;
		Sample.FrameTimeMs = mLastFrameMs;
		Sample.FPS = 1000.0 / mLastFrameMs;
		Sample.WorldUpdateMs = mSectionMs[static_cast<size_t>(EProfileSection::WorldUpdate)];
		Sample.WorldPostUpdateMs = mSectionMs[static_cast<size_t>(EProfileSection::WorldPostUpdate)];
		Sample.CollisionMs = mSectionMs[static_cast<size_t>(EProfileSection::Collision)];
		Sample.RenderMs = mSectionMs[static_cast<size_t>(EProfileSection::Render)];
		Sample.MoveMapMs = MoveMapMs;
		Sample.ObjectCount = mObjectCount;
		Sample.ColliderCount = mColliderCount;
		Sample.RenderCompCount = mRenderCompCount;
		Sample.ActiveZones = mActiveZones;
		Sample.TotalZones = mTotalZones;
		Sample.StreamingEnabled = mStreamingEnabled;

		// 기록 중에는 10프레임마다 메모리 갱신으로 시스템 콜 부하 최소화
		if (mRecordedSamples.empty() || (mFrameIndex % 10) == 0)
		{
			QueryProcessMemoryMB(Sample.PrivateMemMB, Sample.WorkingSetMB);
		}
		else
		{
			Sample.PrivateMemMB = mRecordedSamples.back().PrivateMemMB;
			Sample.WorkingSetMB = mRecordedSamples.back().WorkingSetMB;
		}

		mRecordedSamples.push_back(Sample);
	}

	++mFrameIndex;
}

void CProfiler::AddSectionTimeMs(EProfileSection Section, double Elapsed)
{
	mSectionMs[static_cast<size_t>(Section)] += Elapsed;
}

void CProfiler::SetEntityCounters(size_t ObjectCount, size_t ColliderCount, size_t RenderCompCount)
{
	mObjectCount = ObjectCount;
	mColliderCount = ColliderCount;
	mRenderCompCount = RenderCompCount;
}

void CProfiler::SetZoneInfo(int ActiveZones, int TotalZones, bool StreamingEnabled)
{
	mActiveZones = ActiveZones;
	mTotalZones = TotalZones;
	mStreamingEnabled = StreamingEnabled;
}

bool CProfiler::ToggleRecording()
{
	if (mIsRecording)
	{
		StopAndSaveCsv();
		return false;
	}
	return StartRecording();
}

bool CProfiler::StartRecording()
{
	mRecordedSamples.clear();
	mIsRecording = true;
	return true;
}

std::string CProfiler::StopAndSaveCsv()
{
	mIsRecording = false;

	if (mRecordedSamples.empty())
	{
		return "";
	}

	const char* RootPath = CPathManager::FindPathMultibyte("Root");
	std::string DirPath = (RootPath != nullptr) ? std::string(RootPath) + "PerfReport\\" : "PerfReport\\";
	CreateDirectoryA(DirPath.c_str(), nullptr);

	SYSTEMTIME SysTime = {};
	GetLocalTime(&SysTime);

	char FileName[256] = {};
	sprintf_s(
		FileName,
		"PerfReport_%s_Z%d_%04d%02d%02d_%02d%02d%02d.csv",
		mStreamingEnabled ? "StreamON" : "StreamOFF",
		mTotalZones,
		SysTime.wYear,
		SysTime.wMonth,
		SysTime.wDay,
		SysTime.wHour,
		SysTime.wMinute,
		SysTime.wSecond
	);

	const std::string FullPath = DirPath + FileName;
	FILE* File = nullptr;
	fopen_s(&File, FullPath.c_str(), "wt");
	if (File == nullptr)
	{
		return "";
	}

	fprintf(File, "Frame,FrameTimeMs,FPS,WorldUpdateMs,WorldPostUpdateMs,CollisionMs,RenderMs,MoveMapMs,Objects,Colliders,RenderComps,ActiveZones,TotalZones,PrivateMemMB,WorkingSetMB,Streaming\n");

	double SumFrameMs = 0.0;
	double MaxFrameMs = 0.0;
	double MaxMoveMapMs = 0.0;
	double SumWorldUpdateMs = 0.0;
	double SumWorldPostUpdateMs = 0.0;
	double SumCollisionMs = 0.0;
	double SumRenderMs = 0.0;
	double SumObjects = 0.0;
	double SumColliders = 0.0;
	double SumRenderComps = 0.0;
	double SumPrivateMB = 0.0;

	std::vector<double> SortedFrameMs;
	SortedFrameMs.reserve(mRecordedSamples.size());

	for (const FFrameProfileSample& Sample : mRecordedSamples)
	{
		fprintf(
			File,
			"%llu,%.4f,%.2f,%.4f,%.4f,%.4f,%.4f,%.4f,%zu,%zu,%zu,%d,%d,%.2f,%.2f,%s\n",
			Sample.FrameIndex,
			Sample.FrameTimeMs,
			Sample.FPS,
			Sample.WorldUpdateMs,
			Sample.WorldPostUpdateMs,
			Sample.CollisionMs,
			Sample.RenderMs,
			Sample.MoveMapMs,
			Sample.ObjectCount,
			Sample.ColliderCount,
			Sample.RenderCompCount,
			Sample.ActiveZones,
			Sample.TotalZones,
			Sample.PrivateMemMB,
			Sample.WorkingSetMB,
			Sample.StreamingEnabled ? "ON" : "OFF"
		);

		SumFrameMs += Sample.FrameTimeMs;
		if (Sample.FrameTimeMs > MaxFrameMs)
		{
			MaxFrameMs = Sample.FrameTimeMs;
		}
		if (Sample.MoveMapMs > MaxMoveMapMs)
		{
			MaxMoveMapMs = Sample.MoveMapMs;
		}

		SumWorldUpdateMs += Sample.WorldUpdateMs;
		SumWorldPostUpdateMs += Sample.WorldPostUpdateMs;
		SumCollisionMs += Sample.CollisionMs;
		SumRenderMs += Sample.RenderMs;
		SumObjects += static_cast<double>(Sample.ObjectCount);
		SumColliders += static_cast<double>(Sample.ColliderCount);
		SumRenderComps += static_cast<double>(Sample.RenderCompCount);
		SumPrivateMB += Sample.PrivateMemMB;
		SortedFrameMs.push_back(Sample.FrameTimeMs);
	}

	std::sort(SortedFrameMs.begin(), SortedFrameMs.end(), std::greater<double>());
	const size_t Count = mRecordedSamples.size();
	const size_t OnePercentCount = (std::max<size_t>)(1, Count / 100);
	double SumWorstOnePercentMs = 0.0;
	for (size_t i = 0; i < OnePercentCount; ++i)
	{
		SumWorstOnePercentMs += SortedFrameMs[i];
	}

	const double AvgFrameMs = SumFrameMs / static_cast<double>(Count);
	const double AvgFPS = 1000.0 / AvgFrameMs;
	const double OnePercentLowMs = SumWorstOnePercentMs / static_cast<double>(OnePercentCount);
	const double OnePercentLowFPS = 1000.0 / OnePercentLowMs;

	fprintf(File, "\n# Summary\n");
	fprintf(File, "Metric,Value\n");
	fprintf(File, "Streaming,%s\n", mStreamingEnabled ? "ON" : "OFF");
	fprintf(File, "TotalZones,%d\n", mTotalZones);
	fprintf(File, "RecordedFrames,%zu\n", Count);
	fprintf(File, "AvgFPS,%.2f\n", AvgFPS);
	fprintf(File, "OnePercentLowFPS,%.2f\n", OnePercentLowFPS);
	fprintf(File, "AvgFrameTimeMs,%.4f\n", AvgFrameMs);
	fprintf(File, "MaxFrameTimeMs,%.4f\n", MaxFrameMs);
	fprintf(File, "AvgWorldUpdateMs,%.4f\n", SumWorldUpdateMs / static_cast<double>(Count));
	fprintf(File, "AvgWorldPostUpdateMs,%.4f\n", SumWorldPostUpdateMs / static_cast<double>(Count));
	fprintf(File, "AvgCollisionMs,%.4f\n", SumCollisionMs / static_cast<double>(Count));
	fprintf(File, "AvgRenderMs,%.4f\n", SumRenderMs / static_cast<double>(Count));
	fprintf(File, "MaxMoveMapMs,%.4f\n", MaxMoveMapMs);
	fprintf(File, "AvgObjects,%.1f\n", SumObjects / static_cast<double>(Count));
	fprintf(File, "AvgColliders,%.1f\n", SumColliders / static_cast<double>(Count));
	fprintf(File, "AvgRenderComps,%.1f\n", SumRenderComps / static_cast<double>(Count));
	fprintf(File, "AvgPrivateMemMB,%.2f\n", SumPrivateMB / static_cast<double>(Count));

	fclose(File);

	mLastSavedCsvPath = FileName;
	DEBUG_LOG("[Profiler] Saved CSV: %s", FullPath.c_str());
	return mLastSavedCsvPath;
}

bool CProfiler::IsRecording()
{
	return mIsRecording;
}

FProfilerSnapshot CProfiler::GetSnapshot()
{
	FProfilerSnapshot Snap;
	Snap.CurrentFrameMs = static_cast<float>(mLastFrameMs);
	Snap.CurrentFPS = (mLastFrameMs > 0.0) ? static_cast<float>(1000.0 / mLastFrameMs) : 0.f;

	if (!mRecentFrameMs.empty())
	{
		double SumMs = 0.0;
		double MaxMs = 0.0;
		std::vector<double> Sorted = mRecentFrameMs;
		std::sort(Sorted.begin(), Sorted.end(), std::greater<double>());

		for (double Ms : Sorted)
		{
			SumMs += Ms;
			if (Ms > MaxMs)
			{
				MaxMs = Ms;
			}
		}

		const double AvgMs = SumMs / static_cast<double>(Sorted.size());
		const size_t OnePercentCount = (std::max<size_t>)(1, Sorted.size() / 100);
		double WorstSumMs = 0.0;
		for (size_t i = 0; i < OnePercentCount; ++i)
		{
			WorstSumMs += Sorted[i];
		}
		const double WorstAvgMs = WorstSumMs / static_cast<double>(OnePercentCount);

		Snap.AvgFrameMs = static_cast<float>(AvgMs);
		Snap.AvgFPS = static_cast<float>(1000.0 / AvgMs);
		Snap.OnePercentLowFPS = static_cast<float>(1000.0 / WorstAvgMs);
		Snap.MaxFrameMs = static_cast<float>(MaxMs);
	}

	Snap.WorldUpdateMs = mLastSectionMs[static_cast<size_t>(EProfileSection::WorldUpdate)];
	Snap.WorldPostUpdateMs = mLastSectionMs[static_cast<size_t>(EProfileSection::WorldPostUpdate)];
	Snap.CollisionMs = mLastSectionMs[static_cast<size_t>(EProfileSection::Collision)];
	Snap.RenderMs = mLastSectionMs[static_cast<size_t>(EProfileSection::Render)];
	Snap.LastMoveMapMs = mLastMoveMapMs;
	Snap.MaxMoveMapMs = mMaxMoveMapMs;

	Snap.ObjectCount = mObjectCount;
	Snap.ColliderCount = mColliderCount;
	Snap.RenderCompCount = mRenderCompCount;

	Snap.ActiveZones = mActiveZones;
	Snap.TotalZones = mTotalZones;
	Snap.StreamingEnabled = mStreamingEnabled;

	QueryProcessMemoryMB(Snap.PrivateMemMB, Snap.WorkingSetMB);

	Snap.IsRecording = mIsRecording;
	Snap.RecordedFrames = mRecordedSamples.size();
	Snap.LastSavedCsvPath = mLastSavedCsvPath;

	return Snap;
}

LARGE_INTEGER CProfiler::Now()
{
	LARGE_INTEGER Counter = {};
	QueryPerformanceCounter(&Counter);
	return Counter;
}

double CProfiler::ElapsedMs(const LARGE_INTEGER& Start, const LARGE_INTEGER& End)
{
	if (mFrequency.QuadPart == 0)
	{
		QueryPerformanceFrequency(&mFrequency);
	}
	return static_cast<double>(End.QuadPart - Start.QuadPart) * 1000.0 / static_cast<double>(mFrequency.QuadPart);
}

void CProfiler::QueryProcessMemoryMB(double& OutPrivateMB, double& OutWorkingSetMB)
{
	PROCESS_MEMORY_COUNTERS_EX MemCounters = {};
	MemCounters.cb = sizeof(MemCounters);
	if (GetProcessMemoryInfo(GetCurrentProcess(), reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&MemCounters), sizeof(MemCounters)))
	{
		constexpr double BytesToMB = 1.0 / (1024.0 * 1024.0);
		OutPrivateMB = static_cast<double>(MemCounters.PrivateUsage) * BytesToMB;
		OutWorkingSetMB = static_cast<double>(MemCounters.WorkingSetSize) * BytesToMB;
	}
}

#endif // _DEBUG
