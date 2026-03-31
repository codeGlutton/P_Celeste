#pragma once

#include "Component/SceneComponent.h"

class CMesh;
class CShader;
class CTexture;
class CRenderState;
struct FHairFrameData;

class CMeshComponent;

class CCBufferTransform;
class CCBufferAnimation2D;

namespace EHairTextureType
{
	enum Type
	{
		FrontHair,
		BackHair,
		Count,
	};
}

struct FHairMeshInstancingBuffer
{
	// 행렬
	FVector4 mWVP0;
	FVector4 mWVP1;
	FVector4 mWVP2;
	FVector4 mWVP3;

	// 타일 UV
	FVector2 mLTUV;
	FVector2 mRBUV;

	// 색상
	FVector4 mColor = FVector4::White;
};

class CHairMeshComponent : public CSceneComponent
{
	friend class CGameObject;

protected:
	CHairMeshComponent();
	CHairMeshComponent(const CHairMeshComponent& ref) = default;
	CHairMeshComponent(CHairMeshComponent&& ref) noexcept = default;

public:
	virtual ~CHairMeshComponent() override = default;

protected:
	virtual CHairMeshComponent* Clone()	const override;

public:
	void SetTargetComponent(const std::shared_ptr<CMeshComponent>& Comp);
	void SetPhysicsComponent(const std::shared_ptr<CSceneComponent>& Comp);
	void SetWindEffectOffset(const FVector2& WindEffect)
	{
		mWindEffectOffset = WindEffect;
	}

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;
	virtual void PostUpdate(float DeltaTime) override;
	virtual void Render() override;

public:
	bool SetTexture(EHairTextureType::Type Type, const std::weak_ptr<CTexture>& Texture);
	bool SetTexture(EHairTextureType::Type Type, const std::string& Name);
	bool SetTexture(EHairTextureType::Type Type, const std::string& Name, const TCHAR* FileName, const std::string& PathName = "Texture");
	bool SetTextureFullPath(EHairTextureType::Type Type, const std::string& Name, const TCHAR* FullPath);

public:
	void BlendHairColor(const FVector4& Color);
	void RollBackHairColor(const FVector4& Color);

public:
	void SetHairColor(const FVector4& Color);
	void SetSymmetry(bool IsSymmetry);
	bool GetSymmetry() const;

private:
	bool CreateInstancingBuffer(int32 Size, int32 Count);

	/* 사용 컴포넌트 */
private:
	std::weak_ptr<CMeshComponent> mTargetComp;
	std::weak_ptr<CSceneComponent> mPhysicsComp;

	/* 헤어 데이터 */
public:
	static const std::array<FVector4, 3> mStaticHairColors;

protected:
	static constexpr int32 mBackHairSize = 5;
	static constexpr float mBackHairWeight = 20.f;
	static constexpr float mBackHairDecreaseRate = 0.12f;
	static constexpr float mBackHairMaxLength = 5.f;
	static constexpr float mDeferredFollowLerpRate = 0.1f;

	std::vector<FHairMeshInstancingBuffer> mHairInstancingDatas;
	std::vector<FVector3> mLastBackHairPos;
	FVector4 mHairColor = FVector4::Red;
	FVector3 mWindEffectOffset = FVector3::Zero;
	bool mIsSymmetry = false;

	/* 헤어 이펙트 */
private:
	bool mIsHairBlend = false;
	FVector4 mDesiredHairColor = FVector4::Red;
	float mHairBlendAccTime = 0.f;
	static constexpr float mHairBlendMaxTime = 0.2f;

	bool mIsHairRollBack = false;
	float mHairRollBackAccTime = 0.f;
	static constexpr float mHairRollBackMaxTime = 0.2f;

	/* 리소스 */
private:
	std::weak_ptr<CRenderState>	mHairAlphaBlend;
	std::weak_ptr<CMesh> mHairMesh;
	std::array<std::weak_ptr<CShader>, EHairTextureType::Count> mHairShaders;
	std::array<std::weak_ptr<CTexture>, EHairTextureType::Count> mHairTextures;
	const FHairFrameData* mMetaData;

	/* 기본 데이터 */
protected:
	std::shared_ptr<CCBufferTransform> mTransformCBuffer;
	std::shared_ptr<CCBufferAnimation2D> mAnimCBuffer;

	/* 인스턴싱 데이터 */
protected:
	FVertexBuffer mInstancingBuffer;

private:
	static constexpr float mHairTexScaleOffset = 10.f / 32.f;
};

