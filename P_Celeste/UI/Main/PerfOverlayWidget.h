#pragma once

#include "UI/WidgetContainer.h"

#ifdef _DEBUG

class CImage;
class CTextBlock;

class CPerfOverlayWidget : public CWidgetContainer
{
	friend class CWorldUIManager;

protected:
	CPerfOverlayWidget() = default;

public:
	virtual ~CPerfOverlayWidget() override;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;
	virtual bool CollisionMouse(std::weak_ptr<CWidget>& Result, const FVector2& MousePos) override;

private:
	void ToggleOverlay();
	void ToggleCsvRecord();
	void RefreshText();

private:
	std::weak_ptr<CImage>		mBgImage;
	std::weak_ptr<CTextBlock>	mInfoText;

	bool	mOverlayVisible = false;
	float	mAccRefreshTime = 0.f;

	static constexpr float mRefreshInterval = 0.25f;
};

#endif // _DEBUG
