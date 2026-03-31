#pragma once

#include "../EngineInfo.h"
#include "../Asset/AssetManager.h"
#include "../Asset/Mesh/MeshManager.h"
#include "../Asset/Mesh/Mesh.h"
#include "../Asset/Material/Material.h"
#include "../Asset/Material/MaterialManager.h"
#include "../Asset/Texture/TextureManager.h"
#include "../Asset/Texture/Texture.h"
#include "../Asset/Animation2D/Animation2DManager.h"
#include "../Asset/Animation2D/Animation2D.h"
#include "../Asset/Sound/SoundManager.h"
#include "../Asset/Sound/Sound.h"

class CWorldAssetManager
{
	friend class CWorld;

private:
	CWorldAssetManager();

public:
	~CWorldAssetManager();

public:
	void ClearAsset();
	bool Init();
	void Update(float DeltaTime);

#pragma region Mesh

public:
	bool CreateMesh(const std::string& Name, 
		bool IsWorldIndependent, void* VertexData,
		int VertexSize, int VertexCount,
		D3D11_USAGE VertexUsage, 
		D3D11_PRIMITIVE_TOPOLOGY Primitive,
		void* IndexData = nullptr, int IndexSize = 0,
		int IndexCount = 0,
		DXGI_FORMAT Fmt = DXGI_FORMAT_UNKNOWN,
		D3D11_USAGE IndexUsage = D3D11_USAGE_DEFAULT);
	std::weak_ptr<class CMesh> FindMesh(const std::string& Name);

#pragma endregion Mesh

#pragma region Material
#pragma endregion Material

#pragma region AnimMeta

public:
	bool LoadTexture(const std::string& Name, 
		const TCHAR* FileName,
		const std::string& PathName = "Texture");
	bool LoadTextureFullPath(const std::string& Name,
		const TCHAR* FullPath);
	bool LoadTexture(const std::string& Name,
		const std::vector<const TCHAR*>& FileName,
		const std::string& PathName = "Texture");
	bool LoadTextureFullPath(const std::string& Name,
		const std::vector<const TCHAR*> FullPath);
	bool LoadTextureArray(const std::string& Name,
		const std::vector<const TCHAR*>& FileName,
		const std::string& PathName);
	bool LoadTextureArrayFullPath(const std::string& Name,
		const std::vector<const TCHAR*> FullPath);
	std::weak_ptr<CTexture> FindTexture(
		const std::string& Name);

#pragma endregion AnimMeta

#pragma region Animation2D

public:
	bool CreateAnimation(const std::string& Name);
	std::weak_ptr<CAnimation2D> FindAnimation(
		const std::string& Name);
	bool SetAnimation2DTextureType(const std::string& Name,
		EAnimation2DTextureType Type);
	bool SetTexture(const std::string& AnimationName,
		const std::weak_ptr<class CTexture>& Texture);
	bool SetTexture(const std::string& AnimationName,
		const std::string& Name);
	bool SetTexture(const std::string& AnimationName,
		const std::string& Name,
		const TCHAR* FileName,
		const std::string& PathName = "Texture");
	bool SetTextureFullPath(const std::string& AnimationName,
		const std::string& Name,
		const TCHAR* FullPath);
	bool SetTexture(const std::string& AnimationName,
		const std::string& Name,
		const std::vector<const TCHAR*>& FileName,
		const std::string& PathName = "Texture");
	bool SetTextureFullPath(const std::string& AnimationName,
		const std::string& Name,
		const std::vector<const TCHAR*>& FullPath);
	bool SetTextureArray(const std::string& AnimationName,
		const std::string& Name,
		const std::vector<const TCHAR*>& FileName,
		const std::string& PathName = "Texture");
	bool SetTextureArrayFullPath(const std::string& AnimationName,
		const std::string& Name,
		const std::vector<const TCHAR*>& FullPath);
	bool AddFrame(const std::string& AnimationName,
		const FVector2& Start, const FVector2& Size);
	bool AddFrame(const std::string& AnimationName,
		float StartX, float StartY, float SizeX,
		float SizeY);
	bool AddFrame(const std::string& AnimationName,
		int Count, const FVector2& Start, const FVector2& Size);
	bool AddFrame(const std::string& AnimationName,
		int Count, float StartX, float StartY, float SizeX,
		float SizeY);

public:
	template<typename DataType>
	bool LoadAnimMetaData(const std::string& Name,
		const TCHAR* FileName,
		const std::string& PathName = "Animation");
	template<typename DataType>
	bool LoadAnimMetaDataFullPath(const std::string& Name,
		const TCHAR* FullPath);
	template<typename DataType>
	std::weak_ptr<DataType> FindAnimMetaData(
		const std::string& Name);

#pragma endregion Animation2D

#pragma region Sound

	bool LoadSound(const std::string& Name,
		const std::string& GroupName, bool Loop, const char* FileName,
		const std::string& PathName = "Sound");
	bool LoadSound3D(const std::string& Name,
		const std::string& GroupName, bool Loop, const char* FileName,
		const std::string& PathName = "Sound");
	std::weak_ptr<CSound> FindSound(const std::string& Name);

	bool IsSoundPlaying(const std::string& Name) const;
	void SoundPlay(const std::string& Name, bool Fade = false, float FadeTime = 0.5f);
	void Sound3DPlay(const std::string& Name, const FVector3& Pos, bool Fade = false, float FadeTime = 0.5f);
	void SoundStop(const std::string& Name, bool Fade = false, float FadeTime = 0.5f);
	void SoundPause(const std::string& Name);
	void SoundResume(const std::string& Name);

#pragma endregion Sound

private:
	// AssetType_Name
	std::unordered_map<std::string, std::shared_ptr<class CAsset>>  mAssetMap;
};

template<typename DataType>
bool CWorldAssetManager::LoadAnimMetaData(const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	auto AnimMgr =
		CAssetManager::GetInst()->GetAnimation2DManager().lock();

	if (AnimMgr)
	{
		std::string	Key = "AnimMetaData_" + Name;

		if (!AnimMgr->LoadMetaData<DataType>(Key, FileName, PathName))
			return false;

		auto	iter = mAssetMap.find(Key);

		// 현재 월드의 AssetMap에 추가가 안되어 있을 경우
		if (iter == mAssetMap.end())
		{
			// MeshManager와 mAssetMap에 들어가 있는 Mesh는
			// 공용이다. 그러므로 서로 shared_ptr이므로 같은
			// 객체를 공유하며 카운트가 둘다 1씩 증가하여
			// 2가 되어 있을 것이다.
			mAssetMap.insert(std::make_pair(Key,
				AnimMgr->FindMetaData(Key)));
		}

		return true;
	}

	return false;
}

template<typename DataType>
bool CWorldAssetManager::LoadAnimMetaDataFullPath(const std::string& Name, const TCHAR* FullPath)
{
	auto AnimMgr =
		CAssetManager::GetInst()->GetAnimation2DManager().lock();

	if (AnimMgr)
	{
		std::string	Key = "AnimMetaData_" + Name;

		if (!AnimMgr->LoadMetaData<DataType>(Key, FullPath))
			return false;

		auto	iter = mAssetMap.find(Key);

		// 현재 월드의 AssetMap에 추가가 안되어 있을 경우
		if (iter == mAssetMap.end())
		{
			// MeshManager와 mAssetMap에 들어가 있는 Mesh는
			// 공용이다. 그러므로 서로 shared_ptr이므로 같은
			// 객체를 공유하며 카운트가 둘다 1씩 증가하여
			// 2가 되어 있을 것이다.
			mAssetMap.insert(std::make_pair(Key,
				AnimMgr->FindMetaData(Key)));
		}

		return true;
	}

	return false;
}

template<typename DataType>
inline std::weak_ptr<DataType> CWorldAssetManager::FindAnimMetaData(const std::string& Name)
{
	std::string	Key = "AnimMetaData_" + Name;

	auto	iter = mAssetMap.find(Key);

	// 현재 월드의 AssetMap에 추가가 안되어 있을 경우
	if (iter == mAssetMap.end())
	{
		auto AnimMgr =
			CAssetManager::GetInst()->GetAnimation2DManager().lock();

		auto	AnimMeta = AnimMgr->FindMetaData(Key);

		if (!AnimMeta.expired())
		{
			mAssetMap.insert(std::make_pair(Key, AnimMeta));
		}
		else
		{
			return std::weak_ptr<DataType>();
		}

		iter = mAssetMap.find(Key);
	}

	return std::dynamic_pointer_cast<DataType>(iter->second);
}
