#pragma once

#include "ObjectComponent.h"
#include "Tile.h"

class CMesh;
class CShader;
class CCBufferTransform;
class CCBufferTileMap;
class CTileMapRender;

struct FTileMapInstancingBuffer
{
	// 행렬
	FVector4	WVP0;
	FVector4	WVP1;
	FVector4	WVP2;
	FVector4	WVP3;

	// 타일 UV
	FVector2	LTUV;
	FVector2	RBUV;

	FVector4	Color = FVector4::White;
};

class CTileMapComponent :
    public CObjectComponent
{
	friend class CGameObject;

protected:
	CTileMapComponent();
	CTileMapComponent(const CTileMapComponent& ref);
	CTileMapComponent(CTileMapComponent&& ref)	noexcept;

public:
	virtual ~CTileMapComponent();

public:
	ETileShape GetTileShape()	const
	{
		return mShape;
	}

	FVector2 GetTileScreenSize()	const;
	const FVector2& GetTileSize()	const
	{
		return mTileSize;
	}

	FVector2 GetMapScreenSize()	const;
	const FVector2& GetMapSize()	const
	{
		return mMapSize;
	}

	int GetTileCountX()	const
	{
		return mCountX;
	}

	int GetTileCountY()	const
	{
		return mCountY;
	}

	int GetTileFrameCount()	const
	{
		return (int)mTileFrame.size();
	}

	ETileType GetTileType(int Index);
	int GetTileIndex(const FVector2& Pos)	const;
	int GetTileIndex(const FVector3& Pos)	const;
	int GetTileIndex(float x, float y)	const;
	int GetTileIndexX(const FVector2& Pos)	const;
	int GetTileIndexY(const FVector2& Pos)	const;
	std::weak_ptr<CTile> GetTile(const FVector2& Pos)	const;
	std::weak_ptr<CTile> GetTile(const FVector3& Pos)	const;
	std::weak_ptr<CTile> GetTile(float x, float y)	const;
	std::weak_ptr<CTile> GetTile(int Index)	const
	{
		if (Index < 0 || Index >= (int)mTileList.size())
			return std::weak_ptr<CTile>();

		return mTileList[Index];
	}
	int GetTileFrameIndex(const FVector2& Start, const FVector2& End) const;
	int GetTileFrameIndex(float StartX, float StartY, float EndX, float EndY) const;
	float GetTileRotZ(int Index) const;

	void SetTileFrame(int Index, int Frame);
	void SetTileRotZ(int Index, float angle);

public:
	void SetTileMapRender(
		const std::weak_ptr<CTileMapRender>& Render)
	{
		mTileMapRender = Render;
	}

	void SetTileTextureSize(const FVector2& Size)
	{
		mTileTextureSize = Size;
	}

	void SetTileTextureSize(float x, float y)
	{
		mTileTextureSize.x = x;
		mTileTextureSize.y = y;
	}

public:
	void SetTileOutLineRender(bool Render);
	void SetTileOutLineRender(bool Render,
		int Index);
	void SetOutLineMesh(const std::string& Name);
	void SetOutLineShader(const std::string& Name);
	void SetTileMesh(const std::string& Name);
	void SetTileShader(const std::string& Name);
	bool SetTileTexture(ETileTextureType::Type Type,
		const std::weak_ptr<class CTexture>& Texture);
	bool SetTileTexture(ETileTextureType::Type Type,
		const std::string& Name);
	bool SetTileTexture(ETileTextureType::Type Type,
		const std::string& Name, const TCHAR* FileName,
		const std::string& PathName = "Texture");
	void AddTileFrame(const FVector2& Start,
		const FVector2& End);
	void AddTileFrame(float StartX, float StartY,
		float EndX, float EndY);
	void SetTileFrameAll(int FrameIndex);

public:
	virtual bool Init();
	virtual bool Init(const char* FileName);
	virtual void Update(float DeltaTime);
	virtual void PostUpdate(float DeltaTime);
	virtual void PostRender();
	virtual void Destroy();

protected:
	virtual CTileMapComponent* Clone()	const;

public:
	void RenderTile();
	void RenderTileOutLine();

public:
	void CreateTile(ETileShape Shape, int CountX, int CountY,
		const FVector2& TileSize,
		int TileTextureFrame = -1,
		bool OutLineRender = false,
		bool UsePathFindThread = false
	);
	void ResizeTile(
		int CountX, 
		int CountY,
		int TileTextureFrame = -1,
		bool OutLineRender = false,
		int FrameIndex = -1
	);

private:
	bool CreateInstancingBuffer(int Size, int Count);
	bool SetInstancingData(void* Data, int Count);
	bool CreateLineInstancingBuffer(int Size, int Count);
	bool SetLineInstancingData(void* Data, int Count);

public:
	void Save(const TCHAR* FileName,
		const std::string& PathName = "Asset");
	void SaveFullPath(const TCHAR* FullPath);
	void Load(const TCHAR* FileName,
		const std::string& PathName = "Asset",
		bool UsePathFindThread = false);
	void LoadFullPath(const TCHAR* FullPath,
		bool UsePathFindThread = false);

private:
	int GetTileRenderIndexX(const FVector2& Pos)	const;
	int GetTileRenderIndexY(const FVector2& Pos)	const;

	/* 타일 데이터 */
protected:
	std::vector<std::shared_ptr<CTile>>	mTileList;

	std::vector<FTileMapInstancingBuffer>	mTileIstData;
	std::vector<FTileMapInstancingBuffer>	mTileLineIstData;

	/* 인스턴싱 데이터 */
protected:
	FVertexBuffer	mInstancingBuffer;
	FVertexBuffer	mLineInstancingBuffer;
	int				mInstancingCount = 0;
	int				mLineInstancingCount = 0;

	/* 타일 형태 */
protected:
	ETileShape		mShape = ETileShape::Rect;
	FVector2		mTileSize;
	FVector2		mMapSize;
	int				mCountX = 0;
	int				mCountY = 0;

	/* 랜더 정보 */
protected:
	bool			mTileOutLineRender = false;
	std::weak_ptr<CMesh>		mOutLineMesh;
	std::weak_ptr<CShader>	mOutLineShader;
	std::weak_ptr<CMesh>		mTileMesh;
	std::weak_ptr<CShader>	mTileShader;
	std::shared_ptr<CCBufferTransform>	mTransform;
	std::shared_ptr<CCBufferTileMap>	mCBufferTileMap;
	FVector2	mTileTextureSize;
	std::vector<FTileFrame>		mTileFrame;

	std::weak_ptr<CTileMapRender>		mTileMapRender;

	/* 현재 맵 랜더 정보 */
protected:
	int		mViewStartX = 0;
	int		mViewStartY = 0;
	int		mViewEndX = 0;
	int		mViewEndY = 0;
};

