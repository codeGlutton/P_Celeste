#pragma once

#include "Object/GameObject.h"

class CTexture;
class CMeshComponent;
class CAnimation2DComponent;

class CHairEditorObject : public CGameObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CHairEditorObject();
	CHairEditorObject(const CHairEditorObject& ref) = default;
	CHairEditorObject(CHairEditorObject&& ref) noexcept = default;

public:
	virtual ~CHairEditorObject() override = default;

public:
	virtual bool Init() override;

public:
	void SetTexture(const std::shared_ptr<CTexture>& Texture, int32 FrameIndex);
	void SetOffset(bool IsFrontHair, const FVector2& Offset);
	void SetFlip(bool IsFlip);

protected:
	std::shared_ptr<CMeshComponent> mCharacterMeshComponent;

	std::shared_ptr<CMeshComponent> mFrontHairMeshComponent;
	std::shared_ptr<CAnimation2DComponent> mFrontHairAnimComponent;

	std::shared_ptr<CMeshComponent> mBackHairMeshComponent;
};
