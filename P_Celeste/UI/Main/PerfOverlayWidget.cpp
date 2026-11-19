#include "Pch.h"
#include "PerfOverlayWidget.h"

#ifdef _DEBUG

#include "Profiler.h"
#include "World/World.h"
#include "World/Input.h"
#include "UI/Image.h"
#include "UI/TextBlock.h"

CPerfOverlayWidget::~CPerfOverlayWidget()
{
	// 기록 도중 월드가 전환되거나 종료되면 자동 저장
	if (CProfiler::IsRecording())
	{
		CProfiler::StopAndSaveCsv();
	}
}

bool CPerfOverlayWidget::Init()
{
	CWidgetContainer::Init();

	std::shared_ptr<CImage> BgImage = CreateWidget<CImage>("PerfOverlayBg", 0).lock();
	if (BgImage != nullptr)
	{
		mBgImage = BgImage;
		BgImage->SetPivot(0.f, 0.f);
		BgImage->SetPos(10.f, 10.f);
		BgImage->SetSize(480.f, 225.f);
		BgImage->SetUseInheritedColor(false);
		BgImage->SetWidgetColor(FVector4(0.f, 0.f, 0.f, 0.72f));
	}

	std::shared_ptr<CTextBlock> InfoText = CreateWidget<CTextBlock>("PerfOverlayText", 1).lock();
	if (InfoText != nullptr)
	{
		mInfoText = InfoText;
		InfoText->SetPivot(0.f, 0.f);
		InfoText->SetPos(20.f, 18.f);
		InfoText->SetSize(460.f, 210.f);
		InfoText->SetUseInheritedColor(false);
		InfoText->SetUseInheritedTextColor(false);
		InfoText->SetFont("EngineDefault");
		InfoText->SetFontSize(14.f);
		InfoText->SetTextColor(FVector4::White);
		InfoText->SetAlignH(ETextAlignH::Left);
		InfoText->SetAlignV(ETextAlignV::Top);
		InfoText->EnableOutline(true);
		InfoText->SetOutlineTextColor(FVector4::Black);
		InfoText->SetOutlineWidth(1.f);
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	if (World != nullptr)
	{
		std::shared_ptr<CInput> InputMgr = World->GetInput().lock();
		if (InputMgr != nullptr)
		{
			InputMgr->AddBindKey("TogglePerfOverlay", VK_F5);
			InputMgr->SetBindFunction<CPerfOverlayWidget>("TogglePerfOverlay", EInputType::Press, this, &CPerfOverlayWidget::ToggleOverlay);

			InputMgr->AddBindKey("TogglePerfCsvRecord", VK_F6);
			InputMgr->SetBindFunction<CPerfOverlayWidget>("TogglePerfCsvRecord", EInputType::Press, this, &CPerfOverlayWidget::ToggleCsvRecord);
		}
	}

	SetEnable(false);
	return true;
}

void CPerfOverlayWidget::Update(float DeltaTime)
{
	CWidgetContainer::Update(DeltaTime);

	mAccRefreshTime += DeltaTime;
	if (mAccRefreshTime >= mRefreshInterval)
	{
		mAccRefreshTime = 0.f;
		RefreshText();
	}
}

bool CPerfOverlayWidget::CollisionMouse(std::weak_ptr<CWidget>& Result, const FVector2& MousePos)
{
	// 디버그 HUD가 마우스 이벤트를 가로채지 않도록 통과
	return false;
}

void CPerfOverlayWidget::ToggleOverlay()
{
	mOverlayVisible = !mOverlayVisible;
	SetEnable(mOverlayVisible);

	if (mOverlayVisible)
	{
		mAccRefreshTime = 0.f;
		RefreshText();
	}
}

void CPerfOverlayWidget::ToggleCsvRecord()
{
	CProfiler::ToggleRecording();

	if (mOverlayVisible)
	{
		RefreshText();
	}
}

void CPerfOverlayWidget::RefreshText()
{
	std::shared_ptr<CTextBlock> InfoText = mInfoText.lock();
	if (InfoText == nullptr)
	{
		return;
	}

	const FProfilerSnapshot Snap = CProfiler::GetSnapshot();
	const std::wstring SavedCsvW = StringUtils::ConvertUtf8ToWString(Snap.LastSavedCsvPath);

	wchar_t Buffer[1024] = {};
	swprintf_s(
		Buffer,
		L"[F5] Perf Overlay  |  [F6] CSV Rec: %s\n"
		L"Streaming: %s  (Zones: %d / %d)\n"
		L"FPS: %.1f  (Avg: %.1f | 1%% Low: %.1f)\n"
		L"FrameTime: %.2f ms  (Avg: %.2f | Max: %.2f ms)\n"
		L"CPU(ms): Update %.2f | PostUpd %.2f | Coll %.2f | Render %.2f\n"
		L"MoveMap(ms): Last %.2f | Max %.2f\n"
		L"Entities: Obj %zu | Col %zu | RenderComp %zu\n"
		L"Memory: Private %.1f MB | WorkingSet %.1f MB\n"
		L"Last CSV: %s",
		Snap.IsRecording ? L"REC" : L"IDLE",
		Snap.StreamingEnabled ? L"ON" : L"OFF",
		Snap.ActiveZones,
		Snap.TotalZones,
		Snap.CurrentFPS,
		Snap.AvgFPS,
		Snap.OnePercentLowFPS,
		Snap.CurrentFrameMs,
		Snap.AvgFrameMs,
		Snap.MaxFrameMs,
		Snap.WorldUpdateMs,
		Snap.WorldPostUpdateMs,
		Snap.CollisionMs,
		Snap.RenderMs,
		Snap.LastMoveMapMs,
		Snap.MaxMoveMapMs,
		Snap.ObjectCount,
		Snap.ColliderCount,
		Snap.RenderCompCount,
		Snap.PrivateMemMB,
		Snap.WorkingSetMB,
		SavedCsvW.empty() ? L"-" : SavedCsvW.c_str()
	);

	InfoText->SetText(Buffer);
}

#endif // _DEBUG
