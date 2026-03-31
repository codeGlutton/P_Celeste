#include "Font.h"

#include "../../Device.h"
#include "../../Render/TextGeometryCacheRenderer.h"

CFont::CFont()
{
    mAssetType = EAssetType::Font;
}

CFont::~CFont()
{
    SAFE_RELEASE(mFormat);
}

bool CFont::LoadFont(IDWriteFactory5* Factory, const TCHAR* FontName,
    int Weight, float FontSize, const TCHAR* LocalName, int Stretch)
{
    mFactory = Factory;

    if (FAILED(mFactory->CreateTextFormat(FontName, nullptr,
        (DWRITE_FONT_WEIGHT)Weight, DWRITE_FONT_STYLE_NORMAL,
        (DWRITE_FONT_STRETCH)Stretch, FontSize, LocalName,
        (IDWriteTextFormat**)&mFormat)))
        return false;

    return true;
}

IDWriteTextLayout* CFont::CreateLayout(const TCHAR* Text, int Length,
    float Width, float Height)
{
    IDWriteTextLayout* Layout = nullptr;

    if (FAILED(mFactory->CreateTextLayout(Text, Length, mFormat,
        Width, Height, &Layout)))
        return nullptr;

    return Layout;
}

std::vector<ID2D1PathGeometry*> CFont::CreateGeometry(IDWriteTextLayout* Layout, ID2D1RenderTarget* RenderTarget, OUT std::vector<FVector2>& Offsets)
{
    std::vector<ID2D1PathGeometry*> Gemetries;
    {
        TextGeometryCacheRenderer Renderer(Offsets, Gemetries, RenderTarget);

        if (FAILED(Layout->Draw(NULL, &Renderer, 0, 0)))
        {
            return {};
        }
    }

    return Gemetries;
}

