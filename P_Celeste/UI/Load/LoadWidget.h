#pragma once

#include "UI/WidgetContainer.h"

class CImage;

class CLoadWidget : public CWidgetContainer
{
    friend class CWorldUIManager;
    friend class CWidgetComponent;

protected:
    CLoadWidget() = default;

public:
    virtual ~CLoadWidget() override = default;

public:
    virtual bool Init() override;
};

