#pragma once

#include "UI/WidgetContainer.h"

class CImage;
class CButton;
class CTextBlock;

enum class ETitleScreenChangeType
{
    Enter,
    Select,
    Exit,
    Finish
};

namespace ETitleButtonType
{
    enum Type : uint8
    {
        None = 255,
        Climb = 0,

#ifdef _DEBUG

        Editor,

#endif // _DEBUG

        Exit,
        Count
    };
}

class CTitleWidget : public CWidgetContainer
{
    friend class CWorldUIManager;

protected:
    CTitleWidget();

public:
    virtual ~CTitleWidget() override;

public:
    virtual bool Init() override;
    virtual void Update(float DeltaTime) override;
    virtual void Render() override;

private:
    void OnClickClimb();
    void OnClickEditor();
    void OnClickExit();

private:
    void OnHoverButton(ETitleButtonType::Type Type);
    void OnUnhoverButton(ETitleButtonType::Type Type);

private:
    void InitBlindImages(const FResolution& RS, float ZoomRate);
    void InitClimbButton(const FResolution& RS, float ZoomRate, const FVector3& ButtonStartPos);
    void InitMainButton(
        const FResolution& RS,
        float ZoomRate,
        ETitleButtonType::Type Type,
        const FVector3& ButtonStartPos,
        void(CTitleWidget::* ClickFunc)(),
        const char* TextureName,
        const TCHAR* InnerText
    );

private:
    void UpdateScreenAnimTime(float DeltaTime);
    void UpdateButtonAnimTimes(float DeltaTime);
    void UpdateClimbButtonAnim();
    void UpdateMainButtonAnims();

    /* 레이아웃 변수 */
protected:
    static constexpr float mSelectBlindStepRate = 0.07f;

protected:
    std::weak_ptr<CImage> mInOutBlindImage;
    std::weak_ptr<CWidgetContainer> mSelectBlindContainer;
    std::array<std::weak_ptr<CImage>, 6> mSelectBlindImages;

protected:
    static constexpr float mAllButtonOffsetRateX = 0.1f;

    const FVector2 mClimbButtonSizeRate = { 0.13f, 0.25f };
    const FVector2 mMainButtonSizeRate = { 0.13f, 0.06f };

protected:
    static constexpr float mClimbButtonLineHeightRateY = 0.02f;
    static constexpr float mMainButtonLineHeightRateY = 0.005f;

protected:
    static constexpr float mClimbImageRateY = 0.75f;
    static constexpr float mClimbImagePaddingRateY = 0.05f;
    static constexpr float mClimbTextRateY = 0.2f;
    static constexpr float mClimbTextPaddingRateY = 0.f;

protected:
    static constexpr float mMainImageRateX = 0.3f;
    static constexpr float mMainImagePaddingRateX = 0.05f;
    static constexpr float mMainTextRateX = 0.65f;
    static constexpr float mMainTextPaddingRateY = 0.4f;

protected:
    std::array<std::weak_ptr<CButton>, ETitleButtonType::Count> mMainButtons;
    std::array<std::weak_ptr<CTextBlock>, ETitleButtonType::Count> mMainTexts;

    /* 런타임 관련 변수 */
protected:
    static constexpr float mScreenAnimMaxTime = 0.8f;
    float mAccInOutScreenAnimTime = 0.f;
    float mAccSelectScreenAnimTime = 0.f;

protected:
    ETitleScreenChangeType mScreenChangeType = ETitleScreenChangeType::Enter;
    std::function<void()> mOnFinishScreenAnim = nullptr;

protected:
    static constexpr float mButtonAnimMaxTime = 0.2f;
    std::array<float, ETitleButtonType::Count> mAccButtonAnimTime = {};

protected:
    static constexpr float mHoveredButtonColorMaxTime = 0.1f;
    float mAccHoveredButtonColorTime = 0.f;
    const std::array<FVector4, 2> mHoveredColor = { FVector4::Green, FVector4(1.f, 1.f, 0, 1.f) };
    int32 mHoveredColorIndex = 0;

    ETitleButtonType::Type mHoveredButtonType = ETitleButtonType::None;
};

