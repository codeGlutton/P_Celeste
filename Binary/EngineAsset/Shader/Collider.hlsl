
#include "Share.hlsli"

/* 콜리전 디버깅  */

cbuffer CBCollider : register(b10)
{
    float4 cbColliderColor;
};

float4 main() : SV_TARGET
{
    return cbColliderColor;
}