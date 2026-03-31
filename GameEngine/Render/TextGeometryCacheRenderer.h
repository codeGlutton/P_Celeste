#pragma once

#include "../EngineInfo.h"
#include "../Device.h"

class TextGeometryCacheRenderer : public IDWriteTextRenderer
{
public:
    TextGeometryCacheRenderer(std::vector<FVector2>& Offsets, std::vector<ID2D1PathGeometry*>& Gemetries, ID2D1RenderTarget* RenderTarget) :
        mOffsets(Offsets),
        mGemetries(Gemetries),
        mTarget(RenderTarget)
    {
    }

public:
    IFACEMETHOD(DrawGlyphRun)(
        void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        DWRITE_MEASURING_MODE measuringMode,
        DWRITE_GLYPH_RUN const* glyphRun,
        DWRITE_GLYPH_RUN_DESCRIPTION const* glyphRunDescription,
        IUnknown* clientDrawingEffect
        ) override
    {
        ID2D1PathGeometry* Geometry = nullptr;
        if (FAILED(CDevice::GetInst()->Get2DFactory()->CreatePathGeometry(&Geometry)))
        {
            return S_FALSE;
        }

        ID2D1GeometrySink* Sink;
        Geometry->Open(&Sink);
        glyphRun->fontFace->GetGlyphRunOutline(
            glyphRun->fontEmSize,
            glyphRun->glyphIndices,
            glyphRun->glyphAdvances,
            glyphRun->glyphOffsets,
            glyphRun->glyphCount,
            glyphRun->isSideways,
            glyphRun->bidiLevel % 2,
            Sink
        );
        Sink->Close();
        SAFE_RELEASE(Sink);

        mGemetries.push_back(Geometry);
        mOffsets.push_back({ baselineOriginX, baselineOriginY });

        return S_OK;
    }

public:
    IFACEMETHOD(IsPixelSnappingDisabled)(void*, BOOL* disabled) override
    {
        *disabled = FALSE;
        return S_OK;
    }

    IFACEMETHOD(GetCurrentTransform)(void*, DWRITE_MATRIX* matrix) override
    {
        D2D1_MATRIX_3X2_F mat;
        mTarget->GetTransform(&mat);

        matrix->m11 = mat._11;
        matrix->m12 = mat._12;
        matrix->m21 = mat._21;
        matrix->m22 = mat._22;
        matrix->dx = mat._31;
        matrix->dy = mat._32;

        return S_OK;
    }

    IFACEMETHOD(GetPixelsPerDip)(void*, FLOAT* ppdip) override
    {
        FLOAT dpiX, dpiY;
        mTarget->GetDpi(&dpiX, &dpiY);

        *ppdip = dpiX / 96.0f;
        return S_OK;
    }

    IFACEMETHOD(DrawUnderline)(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        _In_ DWRITE_UNDERLINE const* underline,
        _In_opt_ IUnknown* clientDrawingEffect
        ) override { return S_OK; }
    IFACEMETHOD(DrawStrikethrough)(
        _In_opt_ void* clientDrawingContext,
        FLOAT baselineOriginX,
        FLOAT baselineOriginY,
        _In_ DWRITE_STRIKETHROUGH const* strikethrough,
        _In_opt_ IUnknown* clientDrawingEffect
        ) override { return S_OK; }
    IFACEMETHOD(DrawInlineObject)(
        _In_opt_ void* clientDrawingContext,
        FLOAT originX,
        FLOAT originY,
        _In_ IDWriteInlineObject* inlineObject,
        BOOL isSideways,
        BOOL isRightToLeft,
        _In_opt_ IUnknown* clientDrawingEffect
        ) override { return S_OK; }

    IFACEMETHOD_(ULONG, AddRef)() override { return 1; }
    IFACEMETHOD_(ULONG, Release)() override { return 1; }
    IFACEMETHOD(QueryInterface)(REFIID, void**) override { return E_NOINTERFACE; }

public:
    std::vector<FVector2>& mOffsets;
    std::vector<ID2D1PathGeometry*>& mGemetries;
    ID2D1RenderTarget* mTarget;
};

