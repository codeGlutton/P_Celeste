#pragma once

#include "GameObject/Tile/LayeredTileMapObject.h"

class CTexture;
class CTile;

class CTileEditorObject : public CLayeredTileMapObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CTileEditorObject();
	CTileEditorObject(const CTileEditorObject& ref) = default;
	CTileEditorObject(CTileEditorObject&& ref) noexcept = default;

public:
	virtual ~CTileEditorObject() override = default;

public:
	virtual bool Init() override;

public:
	FTileMapLayer& CeateNewLayer(const std::string& LayerName, const std::string& RenderLayerName);
	void RemoveLayer(const std::string& LayerName);

	void SetLayerAlphaBlend(const std::string& LayerName, bool EnableBlend);
	void SetLayerTexture(
		const std::string& LayerName,
		ETileTextureType::Type TextureType,
		const std::shared_ptr<CTexture>& Texture
	);
	void SetLayerTexture(
		const std::string& LayerName,
		ETileTextureType::Type TextureType,
		const std::string& PathName = "Texture"
	);
	void SetLayerTexture(
		const std::string& LayerName, 
		ETileTextureType::Type TextureType, 
		const std::string& Name, 
		const TCHAR* FileName, 
		const std::string& PathName = "Texture"
	);
	void SetLayerTileFrameSize(
		const std::string& LayerName,
		int32 CountX,
		int32 CountY
	);
	void SetLayerTileFrameSize(
		const std::string& LayerName
	);
	void SetLayerTileSize(
		const std::string& LayerName,
		const FVector2& TileSize,
		int32 CountX,
		int32 CountY
	);
	void SetLayerTileSize(
		const std::string& LayerName
	);
	void ResizeLayer(
		const std::string& LayerName
	);

	void BeginLayer(const std::string& LayerName);

public:
	void SwapRenderLayer(const std::string& LayerName, const std::string& RenderLayerName);

public:
	const FVector2& GetTileSize(const std::string& LayerName) const;
	int32 GetTileFrameCount(const std::string& LayerName) const;
	void GetTileIndex(const std::string& LayerName, const FVector2& Pos, OUT int32& X, OUT int32& Y) const;
	int32 GetTileFrame(const std::string& LayerName, int32& X, int32& Y) const;
	float GetTileRotZ(const std::string& LayerName, int32& X, int32& Y) const;
	bool GetTileRender(const std::string& LayerName, int32& X, int32& Y) const;

public:
	void SetLayerRender(const std::string& LayerName, bool IsRender);
	void FlipLayerEnable(const std::string& LayerName);
	void SetTileHighlight(const std::string& LayerName, int32 X, int32 Y, const FVector4& Color);
	void SetTileFrame(const std::string& LayerName, int32 X, int32 Y, int32 Frame);
	void AddTileFrame(const std::string& LayerName, int32 X, int32 Y, int32 Offset);
	void SetTileRotZ(const std::string& LayerName, int32 X, int32 Y, float Angle);
	void AddTileRotZ(const std::string& LayerName, int32 X, int32 Y, float Offset);
	void SetTileRender(const std::string& LayerName, int32 X, int32 Y, bool IsRender);
	void FlipTileRender(const std::string& LayerName, int32 X, int32 Y);
	void FlipAllTileRender(const std::string& LayerName);

public:
	void LoadMap(const std::string& LayerName, const std::string& PathName = "TileMap");
	void SaveMap(const std::string& LayerName, const std::string& PathName = "TileMap") const;

private:
	std::shared_ptr<CTile> FindTile(const std::string& LayerName, int32 X, int32 Y) const;
};

