#pragma once

#include "Dialogue/DialogueData.h"
#include "UI/WidgetContainer.h"

class CImage;
class CTextBlock;

class IConversable;
struct FDialogueData;

enum class EDialogueWidgetAnimState
{
    None,
    FadeIn,
    FadeOut
};

class CDialogueWidget : public CWidgetContainer
{
    friend class CWorldUIManager;
    friend class CWidgetComponent;

protected:
    CDialogueWidget() = default;

public:
    virtual ~CDialogueWidget() override = default;

public:
    virtual bool Init() override;
    virtual void Update(float DeltaTime) override;

public:
    void StartDialogue(std::shared_ptr<IConversable> Player, std::shared_ptr<IConversable> Other);

private:
    void InitDialogue(const FResolution& RS, float ZoomRate, const FVector3& PanelSize, const FVector3& PanelOffset);
    void InitBinding();

private:
    void MoveNextDialogue();

private:
    void OnStartDialogue();
    void OnNextDialogue();
    void OnEndDialogue();

public:
    std::function<void()> mOnEndDialogue;

    /* 레이아웃 */
protected:
    static constexpr float mDialoguePanelRateX = 0.95f;
    static constexpr float mDialoguePanelRateY = 0.4f;

    static constexpr float mDialoguePanelOffsetRateY = 0.01f;

protected:
    static constexpr float mDialoguePaddingRateX = 0.1f;
    static constexpr float mDialoguePaddingRateY = 0.05f;

    static constexpr float mPortraitRateX = 0.15f;

    static constexpr float mDialogueTextPaddingRateX = 0.05f;
    static constexpr float mDialogueTextPaddingRateY = 0.05f;

    static constexpr float mNextImgRateY = 0.04f;
    static constexpr float mNextImageAnimOffsetRateY = 0.8f;

protected:
    std::weak_ptr<CWidgetContainer> mRootContainer;

    std::weak_ptr<CImage> mPanelImage;
    std::weak_ptr<CImage> mPortraitImage;
    std::weak_ptr<CTextBlock> mDialogueText;

    std::weak_ptr<CImage> mNextImage;

    /* 데이터 */
protected:
    std::weak_ptr<IConversable> mPlayer;
    std::weak_ptr<IConversable> mOther;

    std::weak_ptr<FDialogueContext> mCurContext;
    int32 mCurContextIndex = -1;
    EDialogueWidgetAnimState mAnimState = EDialogueWidgetAnimState::None;

protected:
    static constexpr float mMaxAnimTime = 0.2f;
    float mAccAnimTime = 0.f;

protected:
    bool mIsPortraitAnimLoop = false;

protected:
    static constexpr float mNextArrowMaxAnimTime = 0.75f;
    float mNextArrowAccAnimTime = 0.f;
};

