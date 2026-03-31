#include "Pch.h"
#include "HairMeshComponent.h"

#include "Render/RenderState.h"

#include "Device.h"
#include "World/WorldManager.h"
#include "World/WorldAssetManager.h"
#include "World/CameraManager.h"
#include "Asset/AssetManager.h"
#include "Asset/Shader/ShaderManager.h"
#include "Render/RenderManager.h"

#include "Asset/Shader/CBufferTransform.h"
#include "Asset/Shader/CBufferAnimation2D.h"

#include "Animation/HairMetaData.h"

#include "World/World.h"
#include "Component/MeshComponent.h"
#include "Component/Animation2DComponent.h"

const std::array<FVector4, 3> CHairMeshComponent::mStaticHairColors = { FVector4(0.f, 0.9f, 0.9f, 1.f), FVector4(0.7f, 0.f, 0.f, 1.f), FVector4(1.f, 0.45f, 0.9f, 1.f) };

CHairMeshComponent::CHairMeshComponent()
{
	SetClassType<CHairMeshComponent>();
	mRenderType = EComponentRender::Render;
}

CHairMeshComponent* CHairMeshComponent::Clone() const
{
	return new CHairMeshComponent(*this);
}

void CHairMeshComponent::SetTargetComponent(const std::shared_ptr<CMeshComponent>& Comp)
{
	mTargetComp = Comp;
}

void CHairMeshComponent::SetPhysicsComponent(const std::shared_ptr<CSceneComponent>& Comp)
{
	mPhysicsComp = Comp;
}

bool CHairMeshComponent::Init()
{
	CSceneComponent::Init();

	mAnimCBuffer.reset(new CCBufferAnimation2D);
	mAnimCBuffer->Init();
	mAnimCBuffer->SetLTUV(0.f, 0.f);
	mAnimCBuffer->SetRBUV(1.f, 1.f);
	mAnimCBuffer->SetAnimation2DEnable(false);

	mTransformCBuffer.reset(new CCBufferTransform);
	mTransformCBuffer->Init();

	/* 리소스 등록 */
	{
		std::shared_ptr<CWorld> World = mWorld.lock();
		if (World != nullptr)
		{
			auto AssetMgr = World->GetWorldAssetManager().lock();
			mHairMesh = AssetMgr->FindMesh("RectTex");
		}
		else
		{
			auto MeshMgr = CAssetManager::GetInst()->GetMeshManager().lock();
			mHairMesh = MeshMgr->FindMesh("Mesh_RectTex");
		}

		auto ShaderMgr = CAssetManager::GetInst()->GetShaderManager().lock();
		mHairShaders[EHairTextureType::FrontHair] = ShaderMgr->FindShader("DefaultTexture2D");
		mHairShaders[EHairTextureType::BackHair] = ShaderMgr->FindShader("TileMapInstancing");

		mHairAlphaBlend = CRenderManager::GetInst()->FindRenderState("AlphaBlend");
	}

	/* 인스턴스 초기화 */
	{
		if (CreateInstancingBuffer(sizeof(FHairMeshInstancingBuffer), mBackHairSize * 2) == false)
		{
			return false;
		}

		mHairInstancingDatas.resize(mBackHairSize * 2);
		for (int32 i = 0; i < mHairInstancingDatas.size(); ++i)
		{
			if (i < mBackHairSize)
			{
				mHairInstancingDatas[i].mColor = FVector4::Black;
			}
			mHairInstancingDatas[i].mLTUV = FVector2(0.f, 0.f);
			mHairInstancingDatas[i].mRBUV = FVector2(1.f, 1.f);
		}

		mLastBackHairPos.resize(mBackHairSize, FVector3::Zero);
	}

	return true;
}

void CHairMeshComponent::Update(float DeltaTime)
{
	CSceneComponent::Update(DeltaTime);

	if (mIsHairBlend == true)
	{
		mHairBlendAccTime += DeltaTime;
		if (mHairBlendMaxTime <= mHairBlendAccTime)
		{
			mIsHairBlend = false;
			mHairBlendAccTime = mHairBlendMaxTime;
		}
		mHairColor = MathUtils::Lerp(mHairColor, mDesiredHairColor, mHairBlendAccTime / mHairBlendMaxTime);
	}

	if (mIsHairRollBack == true)
	{
		mHairRollBackAccTime += DeltaTime;
		if (mHairRollBackMaxTime <= mHairRollBackAccTime)
		{
			mIsHairRollBack = false;
		}
	}
}

void CHairMeshComponent::PostUpdate(float DeltaTime)
{
	CSceneComponent::PostUpdate(DeltaTime);

	const FVector4& ResultHairColor = (mIsHairRollBack == true) ? FVector4::White : mHairColor;
	mMetaData = nullptr;

	/* 앞머리 색상 결정 */

	if (mHairMesh.expired() == true)
	{
		return;
	}
	mHairMesh.lock()->SetMaterialBaseColor(0, ResultHairColor);

	/* 타겟 메시와 위치 동기화 */

	if (mPhysicsComp.expired() == true)
	{
		return;
	}
	std::shared_ptr<CSceneComponent> PhysicsComp = mPhysicsComp.lock();
	if (mTargetComp.expired() == true)
	{
		return;
	}
	std::shared_ptr<CMeshComponent> MeshComp = mTargetComp.lock();
	SetWorldPos(MeshComp->GetWorldPos());
	SetWorldScale(MeshComp->GetWorldScale() * mHairTexScaleOffset);

	/* 뒷머리 인스턴싱 데이터 채우기 */

	if (MeshComp->GetAnimComponent().expired() == true)
	{
		return;
	}
	std::shared_ptr<CAnimation2DComponent> AnimComp = MeshComp->GetAnimComponent().lock();

	std::shared_ptr<CWorld> World = mWorld.lock();
	if (World == nullptr)
	{
		World = CWorldManager::GetInst()->GetWorld().lock();
	}
	auto AssetMgr = World->GetWorldAssetManager().lock();
	std::shared_ptr<CHairMetaData> MetaData = AssetMgr->FindAnimMetaData<CHairMetaData>(AnimComp->GetAnimationSplitName()).lock();
	if (MetaData == nullptr)
	{
		return;
	}
	mMetaData = MetaData->GetFrameMetaData<FHairFrameData>(AnimComp->GetAnimationFrame());

	if (mMetaData == nullptr)
	{
		return;
	}

	auto CameraMgr = World->GetCameraManager().lock();
	FMatrix WVPMat, VPMat;
	FMatrix	ScaleMat, TranslateMat;

	VPMat = CameraMgr->GetViewMatrix() * CameraMgr->GetProjMatrix();

	FVector3 HairFollowDir = FVector3::Zero;
	FVector3 PreHairPointPos = mWorldPos + mMetaData->mBackHairOffset * (GetSymmetry() ? FVector2(-1.f, 1.f) : FVector2(1.f, 1.f));
	mLastBackHairPos[0] = PreHairPointPos;
	{
		ScaleMat.Scaling(mWorldScale * 1.2f);
		TranslateMat.Translation(PreHairPointPos);
		WVPMat = ScaleMat * TranslateMat * VPMat;

		mHairInstancingDatas[0].mWVP0 = WVPMat[0];
		mHairInstancingDatas[0].mWVP1 = WVPMat[1];
		mHairInstancingDatas[0].mWVP2 = WVPMat[2];
		mHairInstancingDatas[0].mWVP3 = WVPMat[3];

		ScaleMat.Scaling(mWorldScale);
		WVPMat = ScaleMat * TranslateMat * VPMat;

		mHairInstancingDatas[mBackHairSize].mColor = ResultHairColor;
		mHairInstancingDatas[mBackHairSize].mWVP0 = WVPMat[0];
		mHairInstancingDatas[mBackHairSize].mWVP1 = WVPMat[1];
		mHairInstancingDatas[mBackHairSize].mWVP2 = WVPMat[2];
		mHairInstancingDatas[mBackHairSize].mWVP3 = WVPMat[3];
	}

	for (int32 i = mBackHairSize + 1; i < mHairInstancingDatas.size(); ++i)
	{
		const int Index = i - mBackHairSize;

		mLastBackHairPos[Index] += mWindEffectOffset - FVector3::Axis[EAxis::Y] * mBackHairWeight * GRAVITY2D * (DeltaTime * DeltaTime);
		HairFollowDir = MathUtils::Lerp(mLastBackHairPos[Index], PreHairPointPos, mDeferredFollowLerpRate) - PreHairPointPos;
		const float SegmentLength = HairFollowDir.Length();
		HairFollowDir.Normalize();

		mLastBackHairPos[Index] = PreHairPointPos + HairFollowDir * std::min<float>(mBackHairMaxLength, SegmentLength);
		PreHairPointPos = mLastBackHairPos[Index];

		ScaleMat.Scaling(mWorldScale * (1.2f - Index * mBackHairDecreaseRate));
		TranslateMat.Translation(mLastBackHairPos[Index]);
		WVPMat = ScaleMat * TranslateMat * VPMat;

		mHairInstancingDatas[Index].mWVP0 = WVPMat[0];
		mHairInstancingDatas[Index].mWVP1 = WVPMat[1];
		mHairInstancingDatas[Index].mWVP2 = WVPMat[2];
		mHairInstancingDatas[Index].mWVP3 = WVPMat[3];

		ScaleMat.Scaling(mWorldScale * (1.f - Index * mBackHairDecreaseRate));
		WVPMat = ScaleMat * TranslateMat * VPMat;

		mHairInstancingDatas[i].mColor = ResultHairColor;
		mHairInstancingDatas[i].mWVP0 = WVPMat[0];
		mHairInstancingDatas[i].mWVP1 = WVPMat[1];
		mHairInstancingDatas[i].mWVP2 = WVPMat[2];
		mHairInstancingDatas[i].mWVP3 = WVPMat[3];
	}

	/* 뒷머리 인스턴싱 버퍼 업데이트 */

	if (mInstancingBuffer.Buffer != nullptr)
	{
		ID3D11DeviceContext* Context = CDevice::GetInst()->GetContext();

		D3D11_MAPPED_SUBRESOURCE Subresource = {};
		Context->Map(mInstancingBuffer.Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &Subresource);
		memcpy(Subresource.pData, mHairInstancingDatas.data(), mInstancingBuffer.Size * mInstancingBuffer.Count);
		Context->Unmap(mInstancingBuffer.Buffer, 0);
	}
}

void CHairMeshComponent::Render()
{
	CSceneComponent::Render();

	if (mHairMesh.expired() == true || mMetaData == nullptr)
	{
		return;
	}

	std::shared_ptr<CMesh> Mesh = mHairMesh.lock();
	std::shared_ptr<CRenderState> State = mHairAlphaBlend.lock();
	if (State != nullptr)
	{
		State->SetState();
	}

	/* 앞머리 위치 CBuffer 세팅 */
	{
		FMatrix	ScaleMat, TranslateMat;
		ScaleMat.Scaling(mWorldScale);
		TranslateMat.Translation(mWorldPos + mMetaData->mFrontHairOffset * (GetSymmetry() ? FVector2(-1.f, 1.f) : FVector2(1.f, 1.f)));

		FMatrix WorldMat = ScaleMat * TranslateMat;
		mTransformCBuffer->SetWorldMatrix(WorldMat);

		std::shared_ptr<CWorld> World = mWorld.lock();
		if (World == nullptr)
		{
			World = CWorldManager::GetInst()->GetWorld().lock();
		}

		auto CameraMgr = World->GetCameraManager().lock();
		mTransformCBuffer->SetViewMatrix(CameraMgr->GetViewMatrix());
		mTransformCBuffer->SetProjMatrix(CameraMgr->GetProjMatrix());

		FVector3 PivotSize = mPivot * Mesh->GetMeshSize();
		mTransformCBuffer->SetPivotSize(PivotSize);

		mTransformCBuffer->UpdateBuffer();
	}

	/* 뒷머리 인스턴싱 랜더 */
	if (mHairShaders[EHairTextureType::BackHair].expired() == false && mHairTextures[EHairTextureType::BackHair].expired() == false)
	{
		std::shared_ptr<CShader> Shader = mHairShaders[EHairTextureType::BackHair].lock();
		std::shared_ptr<CTexture> Texture = mHairTextures[EHairTextureType::BackHair].lock();

		mEmptyAnimCBuffer->UpdateBuffer();

		Shader->SetShader();
		Texture->SetShader(1, EShaderBufferType::Pixel, 0);

		Mesh->RenderInstancing(mInstancingBuffer, mInstancingBuffer.Count);
	}

	/* 앞머리 랜더 */
	if (mHairShaders[EHairTextureType::FrontHair].expired() == false && mHairTextures[EHairTextureType::FrontHair].expired() == false)
	{
		std::shared_ptr<CShader> Shader = mHairShaders[EHairTextureType::FrontHair].lock();
		std::shared_ptr<CTexture> Texture = mHairTextures[EHairTextureType::FrontHair].lock();

		mAnimCBuffer->SetTextureSymmetry(mMetaData->mIsFlip ^ mIsSymmetry);
		mAnimCBuffer->UpdateBuffer();

		Shader->SetShader();
		Texture->SetShader(0, EShaderBufferType::Pixel, 0);

		Mesh->Render();
	}

	if (State != nullptr)
	{
		State->ResetState();
	}
}

bool CHairMeshComponent::SetTexture(EHairTextureType::Type Type, const std::weak_ptr<CTexture>& Texture)
{
	if (Type == EHairTextureType::Count)
	{
		return false;
	}

	mHairTextures[Type] = Texture;
	return true;
}

bool CHairMeshComponent::SetTexture(EHairTextureType::Type Type, const std::string& Name)
{
	if (Type == EHairTextureType::Count)
	{
		return false;
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	if (World != nullptr)
	{
		auto AssetMgr = World->GetWorldAssetManager().lock();
		mHairTextures[Type] = AssetMgr->FindTexture(Name);
	}
	else
	{
		auto TexMgr = CAssetManager::GetInst()->GetTextureManager().lock();
		mHairTextures[Type] = TexMgr->FindTexture("Texture_" + Name);
	}
	return true;
}

bool CHairMeshComponent::SetTexture(EHairTextureType::Type Type, const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	if (Type == EHairTextureType::Count)
	{
		return false;
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	if (World != nullptr)
	{
		auto AssetMgr = World->GetWorldAssetManager().lock();
		if (AssetMgr->LoadTexture(Name, FileName, PathName) == false)
		{
			return false;
		}
		mHairTextures[Type] = AssetMgr->FindTexture(Name);
	}
	else
	{
		auto TexMgr = CAssetManager::GetInst()->GetTextureManager().lock();
		if (TexMgr->LoadTexture("Texture_" + Name, FileName, PathName) == false)
		{
			return false;
		}
		mHairTextures[Type] = TexMgr->FindTexture("Texture_" + Name);
	}
	return true;
}

bool CHairMeshComponent::SetTextureFullPath(EHairTextureType::Type Type, const std::string& Name, const TCHAR* FullPath)
{
	if (Type == EHairTextureType::Count)
	{
		return false;
	}

	std::shared_ptr<CWorld> World = mWorld.lock();
	if (World != nullptr)
	{
		auto AssetMgr = World->GetWorldAssetManager().lock();
		if (AssetMgr->LoadTextureFullPath(Name, FullPath) == false)
		{
			return false;
		}
		mHairTextures[Type] = AssetMgr->FindTexture(Name);
	}
	else
	{
		auto TexMgr = CAssetManager::GetInst()->GetTextureManager().lock();
		if (TexMgr->LoadTextureFullPath("Texture_" + Name, FullPath) == false)
		{
			return false;
		}
		mHairTextures[Type] = TexMgr->FindTexture("Texture_" + Name);
	}
	return true;
}

void CHairMeshComponent::BlendHairColor(const FVector4& Color)
{
	mIsHairBlend = true;
	mHairBlendAccTime = 0.f;
	mDesiredHairColor = Color;
}

void CHairMeshComponent::RollBackHairColor(const FVector4& Color)
{
	mIsHairRollBack = true;
	mHairRollBackAccTime = 0.f;
	mHairColor = Color;

	if (mIsHairBlend == true)
	{
		mIsHairBlend = false;
	}
}

void CHairMeshComponent::SetHairColor(const FVector4& Color)
{
	mHairColor = Color;
}

void CHairMeshComponent::SetSymmetry(bool IsSymmetry)
{
	mIsSymmetry = IsSymmetry;
}

bool CHairMeshComponent::GetSymmetry() const
{
	return mIsSymmetry;
}

bool CHairMeshComponent::CreateInstancingBuffer(int32 Size, int32 Count)
{
	SAFE_RELEASE(mInstancingBuffer.Buffer);

	mInstancingBuffer.Size = Size;
	mInstancingBuffer.Count = Count;

	D3D11_BUFFER_DESC	BufferDesc = {};
	BufferDesc.ByteWidth = Size * Count;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(CDevice::GetInst()->GetDevice()->CreateBuffer(&BufferDesc, nullptr, &mInstancingBuffer.Buffer)))
	{
		return false;
	}
	return true;
}

