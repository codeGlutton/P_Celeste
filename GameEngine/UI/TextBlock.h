#pragma once

#include "Widget.h"
#include "../Asset/Font/Font.h"

enum class ETextAlignH
{
	Left,
	Center,
	Right
};

enum class ETextAlignV
{
	Top,
	Middle,
	Bottom
};

class CTextBlock :
    public CWidget
{
	friend class CWorldUIManager;
	friend class CWidgetContainer;
	friend class CWidget;

protected:
	CTextBlock();
	CTextBlock(const CTextBlock& ref);

public:
	virtual ~CTextBlock();

protected:
	ID2D1RenderTarget*		mTarget = nullptr;
	std::wstring			mText;
	std::weak_ptr<CFont>	mFont;
	IDWriteTextLayout*		mLayout = nullptr;
	ID2D1SolidColorBrush*	mTextColor = nullptr;
	FVector4				mReservedTextColor = FVector4::White;
	bool					mUseInheritTextColor = false;
	bool					mTransparency = false;
	float					mOpacity = 1.f;

	bool					mShadow = false;
	ID2D1SolidColorBrush*	mTextShadowColor = nullptr;
	FVector4				mReservedTextShadowColor = FVector4::Black;
	bool					mUseInheritTextShadowColor = false;
	bool					mShadowTransparency = false;
	float					mShadowOpacity = 1.f;
	FVector2				mShadowOffset = { 1.f, 1.f };

	bool					mOutline = false;
	ID2D1SolidColorBrush*	mOutlineColor = nullptr;
	FVector4				mReservedOutlineColor = FVector4::Black;
	bool					mUseInheritTextOutlineColor = false;
	bool					mOutlineTransparency = false;
	float					mOutlineOpacity = 1.f;
	float					mOutlineWidth = 1.f;

	std::vector<FVector2>				mGeometryOffsets = {};
	std::vector<ID2D1PathGeometry*>		mGeometries;

	float					mFontSize = 20.f;
	ETextAlignH				mAlignH = ETextAlignH::Left;
	ETextAlignV				mAlignV = ETextAlignV::Middle;

public:
	const std::wstring& GetText()	const
	{
		return mText;
	}

	const TCHAR* GetTextPtr()	const
	{
		return mText.c_str();
	}

	int GetTextCount()	const
	{
		return (int)mText.length();
	}

	virtual void SetSize(const FVector3& Size)
	{
		CWidget::SetSize(Size);

		CreateTextLayout();
		CreateTextGeometry();
	}

	virtual void SetSize(float x, float y)
	{
		CWidget::SetSize(x, y);

		CreateTextLayout();
		CreateTextGeometry();
	}

	float GetFontSize()	const
	{
		return mFontSize;
	}

public:
	void SetFont(const std::string& FontName);
	void SetText(const TCHAR* Text);
	void SetTextFromInt(int Number);
	void SetTextFromFloat(float Number);
	void AddText(const TCHAR* Text);
	void AddTextFromInt(int Number);
	void AddTextFromFloat(float Number);
	void PopBack();
	void ClearText();
	void SetFontSize(float Size);
	void SetAlignH(ETextAlignH Align);
	void SetAlignV(ETextAlignV Align);

public:
	void SetUseInheritedTextColor(bool UseInheritedColor);
	void EnableTransparency(bool Transparency);
	void SetOpacity(float Opacity);
	void AddOpacity(float Opacity);
	void SetTextColor(unsigned char r, unsigned char g,
		unsigned char b, unsigned char a);
	void SetTextColor(const FVector4& Color);

public:
	void SetUseInheritedShadowColor(bool UseInheritedColor);
	void EnableShadow(bool Enable);
	void EnableShadowTransparency(bool Transparency);
	void SetShadowOpacity(float Opacity);
	void AddShadowOpacity(float Opacity);
	void SetShadowTextColor(unsigned char r, unsigned char g,
		unsigned char b, unsigned char a);
	void SetShadowTextColor(const FVector4& Color);
	void SetShadowOffset(const FVector2& Offset);
	void SetShadowOffset(float x, float y);

public:
	void SetUseInheritedOutlineColor(bool UseInheritedColor);
	void EnableOutline(bool Enable);
	void EnableOutlineTransparency(bool Transparency);
	void SetOutlineOpacity(float Opacity);
	void AddOutlineOpacity(float Opacity);
	void SetOutlineTextColor(unsigned char r, unsigned char g,
		unsigned char b, unsigned char a);
	void SetOutlineTextColor(const FVector4& Color);
	void SetOutlineWidth(float Width);

private:
	void CreateTextLayout();
	void CreateTextGeometry();
	void UpdateBrush(const FVector4& NewColor, ID2D1SolidColorBrush*& Brush);

public:
	virtual bool Init();
	virtual void Update(float DeltaTime);
	virtual void Render();
	virtual bool CollisionMouse(std::weak_ptr<CWidget>& Result,
		const FVector2& MousePos);
	virtual void MouseHovered();
	virtual void MouseUnHovered();
	virtual CTextBlock* Clone()	const;
};

