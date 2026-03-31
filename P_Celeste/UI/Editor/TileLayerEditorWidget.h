#pragma once

#include "UI/WidgetContainer.h"

class CButton;
class CTextBlock;
class CTexture;

class CTileEditorWidget;

class CTileLayerEditorWidget : public CWidgetContainer
{
    friend class CTileEditorWidget;

public:
    virtual bool Init() override;

private:
    void InitFirstLine(const FResolution& RS);

public:
    void SetParentEditor(std::shared_ptr<CTileEditorWidget> ParentEditor);

    void SetLayerIndex(int32 Index)
    {
        mCurLayerIndex = Index;
    }

    int32 GetLayerIndex() const
    {
        return mCurLayerIndex;
    }
    std::string GetLayerName() const
    {
        return "Tile" + std::to_string(mCurLayerIndex);
    }

    void SetRenderLayerIndex(int32 Index)
    {
        mCurRenderLayerIndex = Index;
    }

    int32 GetRenderLayerIndex() const
    {
        return mCurRenderLayerIndex;
    }
    std::string GetRenderLayerName() const
    {
        return "Tile" + std::to_string(mCurRenderLayerIndex);
    }

    void SetIsEnable(bool IsEnable)
    {
        mIsEnable = IsEnable;
    }
    bool GetIsEnable() const
    {
        return mIsEnable;
    }

private:
    void UpdateLayerNameText();
    void UpdateRenderLayerNameText();
    void UpdateEnableText();

    /* 레이아웃 */
private:
    static constexpr float mTextSizeRateX = 0.08f;

    const FVector2 mSizeRate = { 0.4f, 0.04f };
    static constexpr float mPaddingRateX = 0.01f;

private:
    std::shared_ptr<CTextBlock> mLayerNameText;
    std::shared_ptr<CTextBlock> mRenderLayerNameText;
    std::shared_ptr<CTextBlock> mEnableText;

private:
    std::shared_ptr<CButton> mResizeButton;
    std::shared_ptr<CButton> mEnableButton;
    std::shared_ptr<CButton> mRenderLayerUpButton;
    std::shared_ptr<CButton> mRenderLayerDownButton;
    std::shared_ptr<CButton> mRemoveLayerButton;

private:
    std::weak_ptr<CTileEditorWidget> mParentEditor;

    /* 타일맵 */
private:
    int32 mCurLayerIndex = 0;
    int32 mCurRenderLayerIndex = 0;
    bool mIsEnable = true;
};

