#pragma once

#include "EngineSetting.h"

constexpr auto MAX_TILE_RENDER_LAYER = 5;
constexpr auto MAX_FRONT_TILE_RENDER_LAYER = 5;
constexpr auto MAX_BACK_GROUND_RENDER_LAYER = 5;
constexpr auto MAX_FORE_GROUND_RENDER_LAYER = 2;

namespace ERenderObjectLayer
{
    enum Type : int32
    {
        Map = 0,
        BackObject,
        PlayerBackSubObject,
        Player,                 // 유저
        PlayerFrontSubObject,
        FrontObject,
        Count,
    };
}

class CGlobalSetting : public CEngineSetting
{
    friend class CEngine;

private:
    CGlobalSetting() = default;

public:
    ~CGlobalSetting() = default;

public:
    virtual bool Init() override;

private:
    void InitCollisionSettings();
    bool InitMouseWidget();
};

