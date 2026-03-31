#pragma once

#include "UI/WidgetContainer.h"

class CImage;
class CTextBlock;

enum class ESoliloquyWidgetAnimState
{
    None,
    FadeIn,
    FadeOut
};

class CSoliloquyWidget : public CWidgetContainer
{
    friend class CWorldUIManager;
    friend class CWidgetComponent;

protected:
    CSoliloquyWidget() = default;

public:
    virtual ~CSoliloquyWidget() override = default;

public:
    virtual bool Init() override;
    virtual void Update(float DeltaTime) override;

public:
    void StartSoliloquy(const std::vector<std::wstring>& TextDatas);

private:
    void InitSoliloquy(const FResolution& RS, float ZoomRate);
    void InitBinding();

private:
    void MoveNextSoliloquy();

private:
    void OnStartSoliloquy();
    void OnNextSoliloquy();
    void OnEndSoliloquy();

public:
    std::function<void()> mOnEndSoliloquy;

    /* 레이아웃 */
protected:
    static constexpr float mMainFontRateX = 0.025f;
    static constexpr float mMainTextFadeRateY = 0.5f;

protected:
    std::weak_ptr<CWidgetContainer> mRootContainer;
    std::weak_ptr<CImage> mPanelImage;
    std::weak_ptr<CTextBlock> mMainText;

    /* 데이터 */
protected:
    ESoliloquyWidgetAnimState mSceneAnimState = ESoliloquyWidgetAnimState::None;
    static constexpr float mMaxSwapSceneAnimTime = 0.75f;
    float mAccSwapSceneAnimTime = 0.f;

protected:
    std::vector<std::wstring> mShowTextDatas;
    int32 mCurTextDataIndex = -1;

protected:
    ESoliloquyWidgetAnimState mTextAnimState = ESoliloquyWidgetAnimState::None;
    static constexpr float mMaxTextAnimTime = 0.5f;
    float mAccTextAnimTime = 0.f;
};

