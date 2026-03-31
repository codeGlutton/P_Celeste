#include "TextBlock.h"
#include "../Asset/AssetManager.h"
#include "../Asset/Font/FontManager.h"
#include "../Render/RenderManager.h"
#include "../Device.h"

#include "../Asset/Shader/CBufferUIDefault.h"

CTextBlock::CTextBlock()
{
}

CTextBlock::CTextBlock(const CTextBlock& ref)	:
	CWidget(ref)
{
	mTarget = ref.mTarget;
	mText = ref.mText;
	mFont = ref.mFont;
	mTextColor = ref.mTextColor;
	mTransparency = ref.mTransparency;
	mOpacity = ref.mOpacity;

	mShadow = ref.mShadow;
	mTextShadowColor = ref.mTextShadowColor;
	mShadowTransparency = ref.mShadowTransparency;
	mShadowOpacity = ref.mShadowOpacity;
	mShadowOffset = ref.mShadowOffset;

	mFontSize = ref.mFontSize;
	mAlignH = ref.mAlignH;
	mAlignV = ref.mAlignV;

	mLayout = nullptr;

	CreateTextLayout();
	CreateTextGeometry();
}

CTextBlock::~CTextBlock()
{
	SAFE_RELEASE(mLayout);
	for (auto& Geometry : mGeometries)
	{
		SAFE_RELEASE(Geometry);
	}
	mGeometryOffsets.clear();
}

void CTextBlock::SetFont(const std::string& FontName)
{
	auto FontMgr = CAssetManager::GetInst()->GetFontManager().lock();

	mFont = FontMgr->FindFont(FontName);

	CreateTextLayout();
	CreateTextGeometry();
}

void CTextBlock::SetText(const TCHAR* Text)
{
	mText = Text;

	CreateTextLayout();
	CreateTextGeometry();
}

void CTextBlock::SetTextFromInt(int Number)
{
	mText = std::to_wstring(Number);

	CreateTextLayout();
	CreateTextGeometry();
}

void CTextBlock::SetTextFromFloat(float Number)
{
	mText = std::to_wstring(Number);

	CreateTextLayout();
	CreateTextGeometry();
}

void CTextBlock::AddText(const TCHAR* Text)
{
	mText += Text;

	CreateTextLayout();
	CreateTextGeometry();
}

void CTextBlock::AddTextFromInt(int Number)
{
	mText += std::to_wstring(Number);

	CreateTextLayout();
	CreateTextGeometry();
}

void CTextBlock::AddTextFromFloat(float Number)
{
	mText += std::to_wstring(Number);

	CreateTextLayout();
	CreateTextGeometry();
}

void CTextBlock::PopBack()
{
	mText.pop_back();

	CreateTextLayout();
	CreateTextGeometry();
}

void CTextBlock::ClearText()
{
	mText.clear();

	CreateTextLayout();
	CreateTextGeometry();
}

void CTextBlock::SetFontSize(float Size)
{
	mFontSize = Size;

	if (!mLayout)
	{
		CreateTextLayout();
	}
	else
	{
		DWRITE_TEXT_RANGE	Range = {};

		Range.startPosition = 0;
		Range.length = (UINT32)mText.length();

		mLayout->SetFontSize(mFontSize, Range);
	}
	CreateTextGeometry();
}

void CTextBlock::SetAlignH(ETextAlignH Align)
{
	mAlignH = Align;

	if (!mLayout)
	{
		CreateTextLayout();
	}

	else
	{
		switch (mAlignH)
		{
		case ETextAlignH::Left:
			mLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			break;
		case ETextAlignH::Center:
			mLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			break;
		case ETextAlignH::Right:
			mLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
			break;
		}
	}
	CreateTextGeometry();
}

void CTextBlock::SetAlignV(ETextAlignV Align)
{
	mAlignV = Align;

	if (!mLayout)
	{
		CreateTextLayout();
	}

	else
	{
		switch (mAlignV)
		{
		case ETextAlignV::Top:
			mLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			break;
		case ETextAlignV::Middle:
			mLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			break;
		case ETextAlignV::Bottom:
			mLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
			break;
		}
	}
	CreateTextGeometry();
}

void CTextBlock::SetUseInheritedTextColor(bool UseInheritedColor)
{
	mUseInheritTextColor = UseInheritedColor;
}

void CTextBlock::EnableTransparency(bool Transparency)
{
	mTransparency = Transparency;
}

void CTextBlock::SetOpacity(float Opacity)
{
	mOpacity = Opacity;

	if (mOpacity < 0.f)
		mOpacity = 0.f;

	else if (mOpacity > 1.f)
		mOpacity = 1.f;
}

void CTextBlock::AddOpacity(float Opacity)
{
	mOpacity += Opacity;

	if (mOpacity < 0.f)
		mOpacity = 0.f;

	else if (mOpacity > 1.f)
		mOpacity = 1.f;
}

void CTextBlock::SetTextColor(unsigned char r, unsigned char g, 
	unsigned char b, unsigned char a)
{
	mReservedTextColor = FVector4(r, g, b, a);
}

void CTextBlock::SetTextColor(const FVector4& Color)
{
	mReservedTextColor = Color;
}

void CTextBlock::SetUseInheritedShadowColor(bool UseInheritedColor)
{
	mUseInheritTextShadowColor = UseInheritedColor;
}

void CTextBlock::EnableShadow(bool Enable)
{
	mShadow = Enable;
}

void CTextBlock::EnableShadowTransparency(bool Transparency)
{
	mShadowTransparency = Transparency;
}

void CTextBlock::SetShadowOpacity(float Opacity)
{
	mShadowOpacity = Opacity;

	if (mShadowOpacity < 0.f)
		mShadowOpacity = 0.f;

	else if (mShadowOpacity > 1.f)
		mShadowOpacity = 1.f;
}

void CTextBlock::AddShadowOpacity(float Opacity)
{
	mShadowOpacity += Opacity;

	if (mShadowOpacity < 0.f)
		mShadowOpacity = 0.f;

	else if (mShadowOpacity > 1.f)
		mShadowOpacity = 1.f;
}

void CTextBlock::SetShadowTextColor(unsigned char r, unsigned char g,
	unsigned char b, unsigned char a)
{
	mReservedTextShadowColor = FVector4(r, g, b, a);
}

void CTextBlock::SetShadowTextColor(const FVector4& Color)
{
	mReservedTextShadowColor = Color;
}

void CTextBlock::SetShadowOffset(const FVector2& Offset)
{
	mShadowOffset = Offset;
}

void CTextBlock::SetShadowOffset(float x, float y)
{
	mShadowOffset.x = x;
	mShadowOffset.y = y;
}

void CTextBlock::SetUseInheritedOutlineColor(bool UseInheritedColor)
{
	mUseInheritTextOutlineColor = UseInheritedColor;
}

void CTextBlock::EnableOutline(bool Enable)
{
	mOutline = Enable;
}

void CTextBlock::EnableOutlineTransparency(bool Transparency)
{
	mOutlineTransparency = Transparency;
}

void CTextBlock::SetOutlineOpacity(float Opacity)
{
	mOutlineOpacity = Opacity;

	if (mOutlineOpacity < 0.f)
		mOutlineOpacity = 0.f;

	else if (mOutlineOpacity > 1.f)
		mOutlineOpacity = 1.f;
}

void CTextBlock::AddOutlineOpacity(float Opacity)
{
	mOutlineOpacity += Opacity;

	if (mOutlineOpacity < 0.f)
		mOutlineOpacity = 0.f;

	else if (mOutlineOpacity > 1.f)
		mOutlineOpacity = 1.f;
}

void CTextBlock::SetOutlineTextColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	mReservedOutlineColor = FVector4(r, g, b, a);
}

void CTextBlock::SetOutlineTextColor(const FVector4& Color)
{
	mReservedOutlineColor = Color;
}

void CTextBlock::SetOutlineWidth(float Width)
{
	mOutlineWidth = Width;

	if (mOutlineWidth < 1.f)
		mOutlineWidth = 1.f;
}

void CTextBlock::CreateTextLayout()
{
	SAFE_RELEASE(mLayout);

	auto	Font = mFont.lock();

	mLayout = Font->CreateLayout(mText.c_str(),
		(int)mText.length(), mSize.x, mSize.y);

	if (!mLayout)
		return;

	DWRITE_TEXT_RANGE	Range = {};

	Range.startPosition = 0;
	Range.length = (UINT32)mText.length();

	mLayout->SetFontSize(mFontSize, Range);
	mLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);

	switch (mAlignH)
	{
	case ETextAlignH::Left:
		mLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;
	case ETextAlignH::Center:
		mLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	case ETextAlignH::Right:
		mLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;
	}

	switch (mAlignV)
	{
	case ETextAlignV::Top:
		mLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case ETextAlignV::Middle:
		mLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case ETextAlignV::Bottom:
		mLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	}
}

void CTextBlock::CreateTextGeometry()
{
	for (auto& Geometry : mGeometries)
	{
		SAFE_RELEASE(Geometry);
	}
	mGeometryOffsets.clear();

	FVector2 DPI;
	mTarget->GetDpi(&DPI.x, &DPI.y);

	auto Font = mFont.lock();
	mGeometries = Font->CreateGeometry(mLayout, mTarget, OUT mGeometryOffsets);
}

void CTextBlock::UpdateBrush(const FVector4& NewColor, ID2D1SolidColorBrush*& Brush)
{
	auto FontMgr = CAssetManager::GetInst()->GetFontManager().lock();
	FontMgr->CreateFontColor(NewColor);
	Brush = FontMgr->FindFontColor(NewColor);
}

bool CTextBlock::Init()
{
	CWidget::Init();

	mTarget = CDevice::GetInst()->GetCur2DTarget();

	SetFont("EngineDefault");
	SetText(TEXT("Text"));
	SetTextColor(FVector4::White);
	SetShadowTextColor(FVector4::Black);
	SetOutlineTextColor(FVector4::Black);

	CreateTextLayout();
	CreateTextGeometry();

	return true;
}

void CTextBlock::Update(float DeltaTime)
{
	CWidget::Update(DeltaTime);
}

void CTextBlock::Render()
{
	CWidget::Render();

	ID2D1RenderTarget* CurTarget = CDevice::GetInst()->GetCur2DTarget();
	if (CurTarget != mTarget)
	{
		mTarget = CurTarget;

		CreateTextLayout();
		CreateTextGeometry();
	}

	mTarget->BeginDraw();

	D2D1::Matrix3x2F PreTransformMat;
	mTarget->GetTransform(&PreTransformMat);

	D2D1::Matrix3x2F ScaleMat = D2D1::Matrix3x2F::Scale(mRenderScale.x, mRenderScale.y);
	D2D1::Matrix3x2F PivotInvMat = D2D1::Matrix3x2F::Translation(-mPivot.x * mSize.x, -mPivot.y * mSize.y);
	D2D1::Matrix3x2F TransformMat = D2D1::Matrix3x2F::Translation(mRenderPos.x, mRenderPos.y);
	D2D1::Matrix3x2F PSTMat = PivotInvMat * ScaleMat * TransformMat;

	// 그림자를 출력할 경우
	if (mShadow)
	{
		if (mUseInheritTextShadowColor)
		{
			UpdateBrush(mReservedTextShadowColor * mUIDefaultCBuffer->GetWidgetColor(), mTextShadowColor);
		}
		else
		{
			UpdateBrush(mReservedTextShadowColor * mWidgetColor, mTextShadowColor);
		}

		if (mShadowTransparency)
		{
			mTextShadowColor->SetOpacity(mShadowOpacity);
		}
		else
		{
			mTextShadowColor->SetOpacity(1.f);
		}

		///*
		//D2D1_DRAW_TEXT_OPTIONS_NONE : 기본값. 레이아웃 사각형 밖으로
		//나가도 클리핑을 안한다.
		//D2D1_DRAW_TEXT_OPTIONS_NO_SNAP : 픽셀 스탭을 끈다. 스크롤 되는
		//텍스트 출력에 권장된다.
		//D2D1_DRAW_TEXT_OPTIONS_CLIP : 텍스트를 레이아웃 사각형 안에서만
		//보이게 클리핑한다.
		//D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT : 컬러로 폰트 렌더링
		//가능.
		//D2D1_DRAW_TEXT_OPTIONS_DISABLE_COLOR_BITMAP_SNAPPING : 비트맵
		//으로 컬러 그리핑을 처리하는 경우.
		//*/
		//mTarget->DrawTextLayout(ShadowPoint, mLayout,
		//	mTextShadowColor, D2D1_DRAW_TEXT_OPTIONS_CLIP);

		for (int i = 0; i < mGeometries.size(); ++i)
		{
			D2D1_POINT_2F ShadowPoint;
			ShadowPoint.x = mShadowOffset.x + mGeometryOffsets[i].x;
			ShadowPoint.y = mShadowOffset.y + mGeometryOffsets[i].y;

			D2D1::Matrix3x2F TextPivotMat = D2D1::Matrix3x2F::Translation(ShadowPoint.x, ShadowPoint.y);
			mTarget->SetTransform(TextPivotMat * PSTMat);

			mTarget->FillGeometry(mGeometries[i], mTextShadowColor);
		}
	}

	if (mUseInheritTextColor)
	{
		UpdateBrush(mReservedTextColor * mUIDefaultCBuffer->GetWidgetColor(), mTextColor);
	}
	else
	{
		UpdateBrush(mReservedTextColor * mWidgetColor, mTextColor);
	}
	if (mTransparency)
	{
		mTextColor->SetOpacity(mOpacity);
	}
	else
	{
		mTextColor->SetOpacity(1.f);
	}

	if (mOutline)
	{
		if (mUseInheritTextOutlineColor)
		{
			UpdateBrush(mReservedOutlineColor * mUIDefaultCBuffer->GetWidgetColor(), mOutlineColor);
		}
		else
		{
			UpdateBrush(mReservedOutlineColor * mWidgetColor, mOutlineColor);
		}
		if (mOutlineTransparency)
		{
			mOutlineColor->SetOpacity(mOutlineOpacity);
		}
		else
		{
			mOutlineColor->SetOpacity(1.f);
		}
	}

	for (int i = 0; i < mGeometries.size(); ++i)
	{
		D2D1::Matrix3x2F TextPivotMat = D2D1::Matrix3x2F::Translation(mGeometryOffsets[i].x, mGeometryOffsets[i].y);
		mTarget->SetTransform(TextPivotMat * PSTMat);

		if (mOutline)
		{
			mTarget->DrawGeometry(mGeometries[i], mOutlineColor, mOutlineWidth);
		}
		mTarget->FillGeometry(mGeometries[i], mTextColor);
	}
	
	mTarget->SetTransform(PreTransformMat);
	mTarget->EndDraw();
}

bool CTextBlock::CollisionMouse(std::weak_ptr<CWidget>& Result,
	const FVector2& MousePos)
{
	if (!CWidget::CollisionMouse(Result, MousePos))
		return false;

	return true;
}

void CTextBlock::MouseHovered()
{
}

void CTextBlock::MouseUnHovered()
{
}

CTextBlock* CTextBlock::Clone()	const
{
	return new CTextBlock(*this);
}
