#pragma once

#include "UI/WidgetContainer.h"

class CButton;
class CTextBlock;

class CHairEditorWidget;
class CTileEditorWidget;

class CEditorWidget : public CWidgetContainer
{
    friend class CWorldUIManager;

protected:
    CEditorWidget() = default;

public:
    virtual ~CEditorWidget() = default;

public:
    virtual bool Init() override;

private:
    void InitModeText(const FResolution& RS, const FVector3& ButtonStartPos);
    void InitModeButtons(const FResolution& RS, const FVector3& ButtonStartPos);

    void InitModeWidgets(const FResolution& RS, const FVector3& ModeWidgetStartPos);

public:
    const std::shared_ptr<CHairEditorWidget>& GetHairEditWidget() const
    {
        return mHairEditWidget;
    }
    const std::shared_ptr<CTileEditorWidget>& GetTileEditWidget() const
    {
        return mTileEditWidget;
    }

private:
    void OnActivateHairEditMode();
    void OnActivateTileEditMode();

    /* 레이아웃 */
private:
    const FVector2 mTextSizeRate = { 0.15f, 0.04f };
    const FVector2 mAllButtonSizeRate = { 0.1f, 0.05f };
    const FVector2 mAllButtonOffsetRate = { 0.7f, 0.2f };

    const FVector2 mAllModeWidgetOffsetRate = { 0.65f, 0.3f };

private:
    EEditorMode mMode = EEditorMode::Hair;
    std::shared_ptr<CTextBlock> mEditModeText;

private:
    std::shared_ptr<CButton> mHairEditButton;
    std::shared_ptr<CHairEditorWidget> mHairEditWidget;

private:
    std::shared_ptr<CButton> mTileEditButton;
    std::shared_ptr<CTileEditorWidget> mTileEditWidget;
};

