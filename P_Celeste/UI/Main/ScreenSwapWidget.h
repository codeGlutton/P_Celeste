#pragma once

#include "UI/WidgetContainer.h"

class CImage;

enum class EScreenSwapState
{
    None,
    Leave,
    Exit,
    Enter,
};

class CScreenSwapWidget : public CWidgetContainer
{
    friend class CWorldUIManager;

protected:
    CScreenSwapWidget() = default;

public:
    virtual ~CScreenSwapWidget() override = default;

public:
    virtual bool Init() override;
    virtual void Update(float DeltaTime) override;

public:
    void Open();
    void Close();

private:
    void InitBlindImages(const FResolution& RS, float ZoomRate);

private:
    void UpdateScreenAnimTime(float DeltaTime);

public:
    std::function<void()> mOnFinishScreenAnim = nullptr;

    /* 레이아웃 변수 */
protected:
    static constexpr float mSelectBlindStepRate = 0.07f;

protected:
    std::weak_ptr<CWidgetContainer> mBlindContainer;
	std::array<std::weak_ptr<CImage>, 6> mBlindImages;

protected:
    EScreenSwapState mSwapState = EScreenSwapState::Exit;

    /* 런타임 관련 변수 */
protected:
    static constexpr float mScreenAnimMaxTime = 0.8f;
    float mAccScreenAnimTime = 0.f;
};

