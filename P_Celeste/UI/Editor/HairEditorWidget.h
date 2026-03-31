#pragma once

#include "UI/WidgetContainer.h"

class CButton;
class CTextBlock;

class CTexture;
class CHairMetaData;

class CHairEditorObject;

class CHairEditorWidget : public CWidgetContainer
{
public:
    virtual bool Init() override;

private:
    void InitFirstLineText(const FResolution& RS, const FVector3& StartPos);
    void InitSecondLineButtons(const FResolution& RS, const FVector3& StartPos);
    void InitThirdLineButtons(const FResolution& RS, const FVector3& StartPos);
    void InitLastLineButtons(const FResolution& RS, const FVector3& StartPos);
    void InitBackgorundImage(const FResolution& RS, const FVector3& StartPos);

private:
    void OnClickSave();
    void OnClickLoad();

private:
    void OnClickFrontBackToggle();
    void OnClickNextFrame();
    void OnClickPreFrame();

private:
    void OnClickMoveOffsetUp();
    void OnClickMoveOffsetDown();
    void OnClickMoveOffsetLeft();
    void OnClickMoveOffsetRight();

    void OnClickFlip();
    void OnClickPreCopy();

public:
    void CreateEditObjects();
    void DestroyEditObjects();

private:
    FVector2& GetHairOffset();

private:
    void UpdateTogleText();
    void UpdateOffsetText();
    void UpdateFrameText();
    void UpdateFlipText();

    /* 레이아웃 */
private:
    static constexpr float mSizeRateX = 0.3f;

    static constexpr float mTextSizeRateY = 0.05f;
    static constexpr float mAllButtonSizeRateY = 0.05f;

    static constexpr float mPaddingRateX = 0.02f;
    static constexpr float mPaddingRateY = 0.01f;

private:
    std::shared_ptr<CTextBlock> mOffsetText;
    std::shared_ptr<CTextBlock> mFrameText;
    std::shared_ptr<CTextBlock> mToggleText;
    std::shared_ptr<CTextBlock> mFlipText;

private:
    std::shared_ptr<CButton> mSaveButton;
    std::shared_ptr<CButton> mLoadButton;

private:
    std::shared_ptr<CButton> mFrontBackToggleButton;
    std::shared_ptr<CButton> mNextFrameButton;
    std::shared_ptr<CButton> mPreFrameButton;

private:
    std::shared_ptr<CButton> mFlipButton;
    std::shared_ptr<CButton> mPreCopyButton;
    std::shared_ptr<CButton> mUpOffsetButton;
    std::shared_ptr<CButton> mDownOffsetButton;
    std::shared_ptr<CButton> mLeftOffsetButton;
    std::shared_ptr<CButton> mRightOffsetButton;

    /* 캐릭터 */
private:
    std::shared_ptr<CHairEditorObject> mDisplayObject = nullptr;
    std::shared_ptr<CTexture> mCurTexture = nullptr;

    int32 mMetaDataStartIndex = 0;
    int32 mMetaDataFrameCount = 6;
    const TCHAR* mMetaDataFolderName = TEXT("Climb");
    const TCHAR* mTextureFolderName = TEXT("ClimbUp");

    std::wstring mMetaDataFilePath;
    std::vector<std::wstring> mDisplayTextureFilePaths;

    int32 mCurFrameIndex = 0;
    FVector2 mCurFrontOffset = FVector2(0.f, 0.f);
    FVector2 mCurBackOffset = FVector2(0.f, 0.f);
    bool mCurFlip = false;
    bool mIsMoveFrontHair = false;

    /* 저장 데이터 */
private:
    std::shared_ptr<CHairMetaData> mCurMetaData = nullptr;
};

