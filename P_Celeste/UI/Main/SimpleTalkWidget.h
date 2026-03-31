#pragma once

#include "UI/WidgetContainer.h"

enum class ESimpleTalkState : uint8
{
    Dirty = 0,
    FadeOut,
    FadeIn,
    Finish
};

class CImage;
class CTextBlock;

class CSimpleTalkWidget : public CWidgetContainer
{
    friend class CWorldUIManager;
    friend class CWidgetComponent;

protected:
    CSimpleTalkWidget() = default;

public:
    virtual ~CSimpleTalkWidget() override = default;

public:
    virtual bool Init() override;
    virtual void Update(float DeltaTime) override;
    virtual void Render() override;

private:
    void InitText(const FResolution& RS, float ZoomRate);

public:
    const FVector3& GetCurTextBoxSize() const;
    const FVector3& GetMaxTextBoxSize() const;
    const FVector3& GetMaxTextPointSize() const;

public:
    void ChangeText(const std::wstring& MainText, const std::wstring& ContentText);
    void ChangeText(const std::wstring& MainText, const std::wstring& ContentText, const std::string& TextureName);

protected:
    bool IsChangingToText(const std::wstring& MainText, const std::wstring& ContentText) const;

    /* 레이아웃 */
protected:
    static constexpr float mTalkBoxPointRateX = 0.05f;
    static constexpr float mTalkBoxRateX = 0.2f;
    static constexpr float mTalkBoxRateY = 0.17f;

    static constexpr float mTalkSubImageRateX = 0.3f;
    static constexpr float mTalkBoxOutlinePaddingRate = 0.025f;
    static constexpr float mTalkBoxPaddingRate = 0.225f;
    
    FVector4 mOutlineBoxColor = FVector4::White;
    FVector4 mBoxColor = FVector4(0.f, 0.f, 0.2f, 1.f);
    FVector4 mMainTextColor = FVector4(0.27f, 0.35f, 0.65f, 1.f);
    FVector4 mContentTextColor = FVector4::White;
    FVector4 mContentTextShadowColor = FVector4::Blue;

protected:
    std::weak_ptr<CWidgetContainer> mRootContainer;

    std::weak_ptr<CImage> mSubImage;
    std::weak_ptr<CTextBlock> mTalkMainText;
    std::weak_ptr<CTextBlock> mTalkContentText;

    /* 계산 값 */
protected:
    FVector3 mTextBoxMaxSize = FVector3::Zero;
    FVector3 mTextBoxMaxPointSize = FVector3::Zero;

protected:
    ESimpleTalkState mTalkState = ESimpleTalkState::Finish;
    static constexpr float mMaxFadeTime = 0.25f;
    float mAccFadeTime = 0.f;

protected:
    std::wstring mNextMainText;
    std::wstring mNextContentText;

    std::string mNextSubTextureName;
};

