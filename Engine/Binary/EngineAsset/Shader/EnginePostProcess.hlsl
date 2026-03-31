
#include "Share.hlsli"

/// 가중치 4개 * 블러 수
#define MAX_BLUR_COUNT  16

cbuffer CBBlur : register(b10)
{
    // 블러시 참조할 텍스처 한 셀 사이즈
    float2 cbTexelSize;
    // 블러 횟수
    int cbBlurCount;
    float cbBlueEmpty;
    float4 cbBlurWeight[(MAX_BLUR_COUNT + 1 + 3) / 4];
};


struct PS_OUTPUT_COLOR
{
    // 0번 백버퍼에 색상을 출력한다.
    float4 Color : SV_TARGET;
};

Texture2D tbBlendTarget : register(t0);

PS_OUTPUT_COLOR BlurPS(VS_OUTPUT_TEX input)
{
    PS_OUTPUT_COLOR output = (PS_OUTPUT_COLOR) 0;
    
    float4 SrcColor = tbBlendTarget.Sample(sbPoint, input.UV);
        
    if (SrcColor.a == 0.f)
    {
        // 원본픽셀로 출력하고 여기서 리턴된다.
        clip(-1);
    }
    
    SrcColor *= cbBlurWeight[0][0];
    
    int i = 0;
    float4 TargetColor;
    
    // 카운팅 횟수에 따라 픽셀 크기를 늘려가면서 블러 적용
    
    // x 축
    [loop]
    for (i = 1; i <= cbBlurCount; ++i)
    {
        float2 UV = input.UV + float2(cbTexelSize.x * i, 0.f);
        
        TargetColor = tbBlendTarget.Sample(sbPoint, UV);
        
        int Index1 = i / 4;
        int Index2 = i % 4;
        
        //if (TargetColor.a > 0.f)
        SrcColor += TargetColor * cbBlurWeight[Index1][Index2];
        
        UV = input.UV - float2(cbTexelSize.x * i, 0.f);
        
        TargetColor = tbBlendTarget.Sample(sbPoint, UV);
        
        //if (TargetColor.a > 0.f)
        SrcColor += TargetColor * cbBlurWeight[Index1][Index2];
    }
    
    // y 축
    [loop]
    for (i = 1; i <= cbBlurCount; ++i)
    {
        float2 UV = input.UV + float2(0.f, cbTexelSize.y * i);
        
        TargetColor = tbBlendTarget.Sample(sbPoint, UV);
        
        int Index1 = i / 4;
        int Index2 = i % 4;
        
        //if (TargetColor.a > 0.f)
        SrcColor += TargetColor * cbBlurWeight[Index1][Index2];
        
        UV = input.UV - float2(0.f, cbTexelSize.y * i);
        
        TargetColor = tbBlendTarget.Sample(sbPoint, UV);
        
        //if (TargetColor.a > 0.f)
        SrcColor += TargetColor * cbBlurWeight[Index1][Index2];
    }
    
    SrcColor /= 2.f;
    
    output.Color = SrcColor;
    
    return output;
}
