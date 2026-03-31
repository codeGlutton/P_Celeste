#include "Pch.h"
#include "TileEditorObject.h"

#include <tchar.h>
#include <stdlib.h>

#include "Component/TileMapRender.h"
#include "Component/TileMapComponent.h"

#include "Asset/Texture/Texture.h"

#include "Engine.h"
#include "Asset/PathManager.h"
#include "Render/RenderManager.h"

#include "Resource.h"

CTileEditorObject::CTileEditorObject()
{
	SetClassType<CTileEditorObject>();
}

bool CTileEditorObject::Init()
{
	CLayeredTileMapObject::Init();

	CreateComponent<CSceneComponent>("RootScene");

	return true;
}

FTileMapLayer& CTileEditorObject::CeateNewLayer(const std::string& LayerName, const std::string& RenderLayerName)
{
	FTileMapLayer& TileLayer = AddTileLayer(LayerName);
	if (TileLayer.mTileRenderComponent.expired() == false)
	{
		std::shared_ptr<CTileMapRender> TileRenderComp = TileLayer.mTileRenderComponent.lock();
		TileRenderComp->SetRenderLayer(RenderLayerName);
	}
	return TileLayer;
}

void CTileEditorObject::RemoveLayer(const std::string& LayerName)
{
	RemoveTileLayer(LayerName);
}

void CTileEditorObject::SetLayerAlphaBlend(const std::string& LayerName, bool EnableBlend)
{
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileRenderComp != nullptr)
	{
		TileRenderComp->EnableTileAlphaBlend();
	}
}

void CTileEditorObject::SetLayerTexture(const std::string& LayerName, ETileTextureType::Type TextureType, const std::shared_ptr<CTexture>& Texture)
{
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileRenderComp != nullptr)
	{
		TileRenderComp->SetTexture(TextureType, Texture);
	}
}

void CTileEditorObject::SetLayerTexture(const std::string& LayerName, ETileTextureType::Type TextureType, const std::string& PathName)
{
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileRenderComp == nullptr)
	{
		return;
	}

	OPENFILENAME ofn = {};

	const TCHAR Filter[] = TEXT("모든파일\0*.*\0Texture파일\0*.png\0");
	const TCHAR* DefaultPath = CPathManager::FindPath(PathName);
	TCHAR FullPath[MAX_PATH] = {};

	if (TextureType == ETileTextureType::Back)
	{
		ofn.lpstrTitle = TEXT("로드할 배경 텍스처 파일");
	}
	else
	{
		ofn.lpstrTitle = TEXT("로드할 타일 텍스처 파일");
	}
	ofn.lStructSize = sizeof(OPENFILENAME);					// 구조체 크기 지정
	ofn.hwndOwner = CEngine::GetInst()->GetWindowHandle();	// 오너 윈도우 핸들 지정
	ofn.lpstrFilter = Filter;								// 파일 필터 지정
	ofn.lpstrDefExt = TEXT("tlm");							// 기본 확장자 지정
	ofn.lpstrInitialDir = DefaultPath;						// 열어줄 기본 폴더를 지정한다.
	ofn.nMaxFile = MAX_PATH;								// 배열 최대 개수
	ofn.lpstrFile = FullPath;								// 선택된 경로

	ShowCursor(TRUE);

	if (GetSaveFileName(&ofn) != 0)
	{
		TCHAR drive[_MAX_DRIVE];
		TCHAR dir[_MAX_DIR];
		TCHAR fname[_MAX_FNAME];
		TCHAR ext[_MAX_EXT];

		_tsplitpath_s(FullPath, drive, dir, fname, ext);

		std::string FileName = StringUtils::ConvertWStringToUtf8(fname);
		TileRenderComp->SetTextureFullPath(TextureType, FileName, FullPath);
	}

	ShowCursor(FALSE);
}

void CTileEditorObject::SetLayerTexture(const std::string& LayerName, ETileTextureType::Type TextureType, const std::string& Name, const TCHAR* FileName, const std::string& PathName)
{
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileRenderComp != nullptr)
	{
		TileRenderComp->SetTexture(TextureType, Name, FileName, PathName);
	}
}

void CTileEditorObject::SetLayerTileFrameSize(const std::string& LayerName, int32 CountX, int32 CountY)
{
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileRenderComp == nullptr)
	{
		return;
	}

	std::shared_ptr<CTexture> Textrue = TileRenderComp->GetTexture(ETileTextureType::Tile).lock();
	if (Textrue == nullptr)
	{
		return;
	}

	const FVector2 TileSize = {
		Textrue->GetTexture()->Width / static_cast<float>(CountX),
		Textrue->GetTexture()->Height / static_cast<float>(CountY)
	};
	for (int32 y = 0; y < CountY; ++y)
	{
		for (int32 x = 0; x < CountX; ++x)
		{
			TileRenderComp->AddTileFrame(x * TileSize.x, y * TileSize.y, (x + 1) * TileSize.x, (y + 1) * TileSize.y);
		}
	}
}

void CTileEditorObject::SetLayerTileFrameSize(const std::string& LayerName)
{
	FSizeDiaglogData TileFrameSizeData;
	TileFrameSizeData.mTitle = L"아틀라스의 타일 갯수를 입력하시오";
	TileFrameSizeData.mSizeX = 0;
	TileFrameSizeData.mSizeY = 0;

	if (DialogBoxParam(
		CEngine::GetInst()->GetWindowInstance(),
		MAKEINTRESOURCE(IDD_TILE_SIZE_DIALOG),
		CEngine::GetInst()->GetWindowHandle(),
		TileSizeDlgProc,
		(LPARAM)&TileFrameSizeData) == IDOK)
	{
		SetLayerTileFrameSize(LayerName, TileFrameSizeData.mSizeX, TileFrameSizeData.mSizeY);
	}
}

void CTileEditorObject::SetLayerTileSize(const std::string& LayerName, const FVector2& TileSize, int32 CountX, int32 CountY)
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	if (TileMapComp == nullptr)
	{
		return;
	}

	TileMapComp->CreateTile(ETileShape::Rect, CountX, CountY, FVector2(TileSize.x, TileSize.y));
	TileMapComp->SetTileFrameAll(0);
}

void CTileEditorObject::SetLayerTileSize(const std::string& LayerName)
{
	FSizeDiaglogData TileSizeData;
	TileSizeData.mTitle = L"단일 타일 크기를 입력하시오";
	TileSizeData.mSizeX = 0;
	TileSizeData.mSizeY = 0;

	if (DialogBoxParam(
		CEngine::GetInst()->GetWindowInstance(),
		MAKEINTRESOURCE(IDD_TILE_SIZE_DIALOG),
		CEngine::GetInst()->GetWindowHandle(),
		TileSizeDlgProc,
		(LPARAM)&TileSizeData) != IDOK)
	{
		return;
	}

	FSizeDiaglogData TileCountData;
	TileCountData.mTitle = L"생성될 타일 갯수를 입력하시오";
	TileCountData.mSizeX = 0;
	TileCountData.mSizeY = 0;

	if (DialogBoxParam(
		CEngine::GetInst()->GetWindowInstance(),
		MAKEINTRESOURCE(IDD_TILE_SIZE_DIALOG),
		CEngine::GetInst()->GetWindowHandle(),
		TileSizeDlgProc,
		(LPARAM)&TileCountData) != IDOK)
	{
		return;
	}

	SetLayerTileSize(LayerName, FVector2(TileSizeData.mSizeX, TileSizeData.mSizeY), TileCountData.mSizeX, TileCountData.mSizeY);
}

void CTileEditorObject::ResizeLayer(const std::string& LayerName)
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	if (TileMapComp == nullptr)
	{
		return;
	}

	FSizeDiaglogData TileCountData;
	TileCountData.mTitle = L"생성될 타일 갯수를 입력하시오";
	TileCountData.mSizeX = 0;
	TileCountData.mSizeY = 0;

	if (DialogBoxParam(
		CEngine::GetInst()->GetWindowInstance(),
		MAKEINTRESOURCE(IDD_TILE_SIZE_DIALOG),
		CEngine::GetInst()->GetWindowHandle(),
		TileSizeDlgProc,
		(LPARAM)&TileCountData) != IDOK)
	{
		return;
	}

	TileMapComp->ResizeTile(TileCountData.mSizeX, TileCountData.mSizeY, -1, false, 0);
}

void CTileEditorObject::BeginLayer(const std::string& LayerName)
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileMapComp == nullptr || TileRenderComp == nullptr)
	{
		return;
	}

	TileMapComp->Begin();
	TileRenderComp->Begin();
}

void CTileEditorObject::SwapRenderLayer(const std::string& LayerName, const std::string& RenderLayerName)
{
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileRenderComp == nullptr)
	{
		return;
	}
	int32 PreRenderLayerIndex = TileRenderComp->GetRenderLayer();
	TileRenderComp->SetRenderLayer(RenderLayerName);

	CRenderManager::GetInst()->SwapRenderLayer(PreRenderLayerIndex, TileRenderComp);
}

const FVector2& CTileEditorObject::GetTileSize(const std::string& LayerName) const
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	return TileMapComp->GetTileSize();
}

int32 CTileEditorObject::GetTileFrameCount(const std::string& LayerName) const
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	return TileMapComp->GetTileFrameCount();
}

void CTileEditorObject::GetTileIndex(const std::string& LayerName, const FVector2& Pos, OUT int32& X, OUT int32& Y) const
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	if (TileMapComp != nullptr)
	{
		std::shared_ptr<CTile> Tile = TileMapComp->GetTile(Pos).lock();
		if (Tile == nullptr)
		{
			X = Y = -1;
		}
		else
		{
			X = Tile->GetIndexX();
			Y = Tile->GetIndexY();
		}
	}
}

int32 CTileEditorObject::GetTileFrame(const std::string& LayerName, int32& X, int32& Y) const
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr || TileMapComp == nullptr)
	{
		return -1;
	}

	return TileMapComp->GetTileFrameIndex(Tile->GetFrameStart(), Tile->GetFrameEnd());
}

float CTileEditorObject::GetTileRotZ(const std::string& LayerName, int32& X, int32& Y) const
{
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr)
	{
		return -1.f;
	}
	return Tile->GetRotZ();
}

bool CTileEditorObject::GetTileRender(const std::string& LayerName, int32& X, int32& Y) const
{
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr)
	{
		return false;
	}
	return Tile->GetRender();
}

void CTileEditorObject::SetLayerRender(const std::string& LayerName, bool IsRender)
{
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileRenderComp != nullptr)
	{
		TileRenderComp->SetEnable(IsRender);
	}
}

void CTileEditorObject::FlipLayerEnable(const std::string& LayerName)
{
	std::shared_ptr<CTileMapRender> TileRenderComp = GetTileRenderComponent(LayerName);
	if (TileRenderComp != nullptr)
	{
		TileRenderComp->SetEnable(!TileRenderComp->GetEnable());
	}
}

void CTileEditorObject::SetTileHighlight(const std::string& LayerName, int32 X, int32 Y, const FVector4& Color)
{
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr)
	{
		return;
	}

	if (Color.w > 0.f)
	{
		Tile->SetOutLineRender(true);
		Tile->SetOutLineColor(Color);
	}
	else
	{
		Tile->SetOutLineRender(false);
		Tile->SetOutLineColor(FVector4::White);
	}
}

void CTileEditorObject::SetTileFrame(const std::string& LayerName, int32 X, int32 Y, int32 Frame)
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr || TileMapComp == nullptr)
	{
		return;
	}

	TileMapComp->SetTileFrame(Tile->GetIndex(), Frame);
}

void CTileEditorObject::AddTileFrame(const std::string& LayerName, int32 X, int32 Y, int32 Offset)
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr || TileMapComp == nullptr)
	{
		return;
	}

	int32 FrameIndex = TileMapComp->GetTileFrameIndex(Tile->GetFrameStart(), Tile->GetFrameEnd());
	if (FrameIndex == -1)
	{
		return;
	}

	int32 NewFrameIndex = (FrameIndex + Offset + TileMapComp->GetTileFrameCount()) % TileMapComp->GetTileFrameCount();
	TileMapComp->SetTileFrame(Tile->GetIndex(), NewFrameIndex);
}

void CTileEditorObject::SetTileRotZ(const std::string& LayerName, int32 X, int32 Y, float Angle)
{
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr)
	{
		return;
	}

	Tile->SetRotZ(Angle);
}

void CTileEditorObject::AddTileRotZ(const std::string& LayerName, int32 X, int32 Y, float Offset)
{
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr)
	{
		return;
	}

	Tile->SetRotZ(std::fmodf(Tile->GetRotZ() + Offset + 360.f, 360.f));
}

void CTileEditorObject::SetTileRender(const std::string& LayerName, int32 X, int32 Y, bool IsRender)
{
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr)
	{
		return;
	}

	Tile->SetRender(IsRender);
}

void CTileEditorObject::FlipTileRender(const std::string& LayerName, int32 X, int32 Y)
{
	std::shared_ptr<CTile> Tile = FindTile(LayerName, X, Y);
	if (Tile == nullptr)
	{
		return;
	}

	Tile->SetRender(!Tile->GetRender());
}

void CTileEditorObject::FlipAllTileRender(const std::string& LayerName)
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	if (TileMapComp == nullptr)
	{
		return;
	}

	const int32 CountY = TileMapComp->GetTileCountY();
	const int32 CountX = TileMapComp->GetTileCountX();

	for (int32 x = 0; x < CountX; ++x)
	{
		for (int32 y = 0; y < CountY; ++y)
		{
			std::shared_ptr<CTile> Tile = FindTile(LayerName, x, y);
			Tile->SetRender(!Tile->GetRender());
		}
	}
}

void CTileEditorObject::LoadMap(const std::string& LayerName, const std::string& PathName)
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	if (TileMapComp == nullptr)
	{
		return;
	}

	OPENFILENAME ofn = {};

	const TCHAR Filter[] = TEXT("모든파일\0*.*\0Map파일\0*.tlm\0");
	const TCHAR* DefaultPath = CPathManager::FindPath(PathName);
	TCHAR FullPath[MAX_PATH] = {};

	ofn.lpstrTitle = TEXT("로드할 타일맵 파일");
	ofn.lStructSize = sizeof(OPENFILENAME);					// 구조체 크기 지정
	ofn.hwndOwner = CEngine::GetInst()->GetWindowHandle();	// 오너 윈도우 핸들 지정
	ofn.lpstrFilter = Filter;								// 파일 필터 지정
	ofn.lpstrDefExt = TEXT("tlm");							// 기본 확장자 지정
	ofn.lpstrInitialDir = DefaultPath;						// 열어줄 기본 폴더를 지정한다.
	ofn.nMaxFile = MAX_PATH;								// 배열 최대 개수
	ofn.lpstrFile = FullPath;								// 선택된 경로

	ShowCursor(TRUE);

	if (GetSaveFileName(&ofn) != 0)
	{
		TileMapComp->LoadFullPath(FullPath);
	}

	ShowCursor(FALSE);
}

void CTileEditorObject::SaveMap(const std::string& LayerName, const std::string& PathName) const
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	if (TileMapComp == nullptr)
	{
		return;
	}

	OPENFILENAME ofn = {};

	const TCHAR Filter[] = TEXT("모든파일\0*.*\0Map파일\0*.tlm\0");
	const TCHAR* DefaultPath = CPathManager::FindPath(PathName);
	TCHAR FullPath[MAX_PATH] = {};

	std::wstring TitleText = StringUtils::ConvertUtf8ToWString(LayerName) + TEXT("를 저장할 타일맵 파일");
	ofn.lpstrTitle = TitleText.c_str();
	ofn.lStructSize = sizeof(OPENFILENAME);					// 구조체 크기 지정
	ofn.hwndOwner = CEngine::GetInst()->GetWindowHandle();	// 오너 윈도우 핸들 지정
	ofn.lpstrFilter = Filter;								// 파일 필터 지정
	ofn.lpstrDefExt = TEXT("tlm");							// 기본 확장자 지정
	ofn.lpstrInitialDir = DefaultPath;						// 열어줄 기본 폴더를 지정한다.
	ofn.nMaxFile = MAX_PATH;								// 배열 최대 개수
	ofn.lpstrFile = FullPath;								// 선택된 경로

	ShowCursor(TRUE);

	if (GetSaveFileName(&ofn) != 0)
	{
		TileMapComp->SaveFullPath(FullPath);
	}

	ShowCursor(FALSE);
}

std::shared_ptr<CTile> CTileEditorObject::FindTile(const std::string& LayerName, int32 X, int32 Y) const
{
	std::shared_ptr<CTileMapComponent> TileMapComp = GetTileMapComponent(LayerName);
	if (TileMapComp != nullptr)
	{
		int32 Index = TileMapComp->GetTileCountX() * Y + X;
		return TileMapComp->GetTile(Index).lock();
	}
	return nullptr;
}

