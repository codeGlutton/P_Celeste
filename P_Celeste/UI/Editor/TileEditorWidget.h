#pragma once

#include "UI/WidgetContainer.h"

class CButton;
class CTextBlock;

class CTexture;

class CTile;
class CGameObject;
class CTileEditorObject;
class CTileLayerEditorWidget;

class CTileEditorWidget : public CWidgetContainer
{
public:
    virtual bool Init() override;

private:
    void InitFirstLineButtons(const FResolution& RS, const FVector3& StartPos); // 현 레이어와 타일
    void InitSecondLineButtons(const FResolution& RS, const FVector3& StartPos); // 세이브 로드 추가
    void InitThirdLineButtons(const FResolution& RS, const FVector3& StartPos); // 렌더 여부, 프레임
    void InitLastLineButtons(const FResolution& RS, const FVector3& StartPos); // 회전
    void InitBackgorundImage(const FResolution& RS, const FVector3& StartPos);

private:
    bool IsValidLayer() const
    {
        return mCurLayer.empty() == false;
    }

    bool IsValidTileIndex() const
    {
        return mCurTileIndexX >= 0 && mCurTileIndexY >= 0 && IsValidLayer();
    }

    bool IsValidCopyData() const;

public:
    void OnClickSelectionLock();

public:
    void OnClickSave();
    void OnClickLoad();
    void OnClickAddLayer();

public:
    void OnClickNextTileFrame();
    void OnClickPreTileFrame();
    void OnClickRenderTile();

public:
    void OnClickNextTileRotZ();
    void OnClickPreTileRotZ();
    void OnClickAllFlipTile();

public:
    void OnClickResizeLayer(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget);
    void OnClickEnableLayer(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget);
    void OnClickRenderLayerUp(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget);
    void OnClickRenderLayerDown(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget);
    void OnClickRemoveLayer(const std::string& LayerName, std::shared_ptr<CTileLayerEditorWidget> LayerWidget);

private:
    void UpdateSelectionLockText();
    void UpdateLayerText();
    void UpdateTileIndexText();
    void UpdateFrameText();
    void UpdateRotZText();
    void UpdateRenderEnableText();

    void UpdateLayerWidgetLayouts();

public:
    void ShowEditObjects();
    void HideEditObjects();

    void ResetTileSelection();
    void CheckTileUnderMouse();

    void ResetCopyData();
    void CopyFrameAndRotZ();
    void PasteFrameAndRotZ();

    /* 레이아웃 */
private:
    static constexpr float mSizeRateX = 0.3f;

    static constexpr float mTextSizeRateY = 0.05f;
    static constexpr float mAllButtonSizeRateY = 0.05f;

    static constexpr float mPaddingRateX = 0.02f;
    static constexpr float mPaddingRateY = 0.01f;

    const FVector2 mLayerWidgetSizeRate = { 0.4f, 0.05f };
    FVector2 mLayerWidgetStartOffset;

    const FVector4 mHightLightColor = FVector4(1.f, 0.5f, 0.5f, 1.f);

private:
    std::shared_ptr<CWidgetContainer> mLayerContainer;
    std::list<std::shared_ptr<CTileLayerEditorWidget>> mLayerEditorWidgets;

private:
    std::shared_ptr<CTextBlock> mSelectionLockText;

    std::shared_ptr<CTextBlock> mLayerText;
    std::shared_ptr<CTextBlock> mTileIndexText;

    std::shared_ptr<CTextBlock> mFrameText;
    std::shared_ptr<CTextBlock> mRotZText;

    std::shared_ptr<CTextBlock> mRenderEnableText;

private:
    std::shared_ptr<CButton> mSelectionLockButton;

    std::shared_ptr<CButton> mSaveButton;
    std::shared_ptr<CButton> mLoadButton;
    std::shared_ptr<CButton> mAddLayerButton;

private:
    std::shared_ptr<CButton> mRenderEnableButton;

    std::shared_ptr<CButton> mTileFramePreButton;
    std::shared_ptr<CButton> mTileFrameNextButton;

    std::shared_ptr<CButton> mAllRenderFlipButton;

    std::shared_ptr<CButton> mTileRotPreButton;
    std::shared_ptr<CButton> mTileRotNextButton;

    /* 타일맵 */
private:
    int32 mMaxLayerNameIndex = 0;

    std::string mCurLayer;
    int32 mCurTileIndexX = -1;
    int32 mCurTileIndexY = -1;
    bool mIsSelectionLock = false;

    int32 mCopyFrame = -1;
    float mCopyRotZ = -1.f;

    /* 캐릭터 */
private:
    std::shared_ptr<CTileEditorObject> mDisplayObject = nullptr;
    std::shared_ptr<CGameObject> mTestObject = nullptr;
};

