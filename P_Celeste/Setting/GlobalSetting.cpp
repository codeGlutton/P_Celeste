#include "Pch.h"
#include "GlobalSetting.h"

#include "CollisionInfoManager.h"
#include "Render/RenderManager.h"

bool CGlobalSetting::Init()
{
	InitCollisionSettings();
	return InitMouseWidget();
}

void CGlobalSetting::InitCollisionSettings()
{
	/* 콜리전 채널 */

	CCollisionInfoManager::GetInst()->CreateChannel("Blocker", ECollisionInteraction::Ignore);
	CCollisionInfoManager::GetInst()->SetProfileInteraction("Player", "Blocker", ECollisionInteraction::Block);
	CCollisionInfoManager::GetInst()->CreateProfile("Blocker", "Blocker", true);

	CCollisionInfoManager::GetInst()->CreateChannel("Trigger", ECollisionInteraction::Ignore);
	CCollisionInfoManager::GetInst()->SetProfileInteraction("Player", "Trigger", ECollisionInteraction::Overlap);
	CCollisionInfoManager::GetInst()->CreateProfile("Item", "Trigger", true);
	CCollisionInfoManager::GetInst()->CreateProfile("EventTrigger", "Trigger", true);

	CCollisionInfoManager::GetInst()->CreateChannel("Sensor", ECollisionInteraction::Overlap);
	CCollisionInfoManager::GetInst()->CreateProfile("EnvSensor", "Sensor", true);
	CCollisionInfoManager::GetInst()->SetProfileInteraction("EnvSensor", "Player", ECollisionInteraction::Ignore);
	CCollisionInfoManager::GetInst()->SetProfileInteraction("EnvSensor", "Sensor", ECollisionInteraction::Ignore);
	CCollisionInfoManager::GetInst()->SetProfileInteraction("EnvSensor", "NPC", ECollisionInteraction::Ignore);

	/* 렌더 채널 */

	for (int32 i = 0; i < MAX_BACK_GROUND_RENDER_LAYER; ++i)
	{
		CRenderManager::GetInst()->CreateLayer("Background" + std::to_string(i), i - MAX_BACK_GROUND_RENDER_LAYER - MAX_TILE_RENDER_LAYER, ERenderListSort::Alpha);
	}
	for (int32 i = 0; i < MAX_TILE_RENDER_LAYER; ++i)
	{
		CRenderManager::GetInst()->CreateLayer("Tile" + std::to_string(i), i - MAX_TILE_RENDER_LAYER, ERenderListSort::Alpha);
	}
	CRenderManager::GetInst()->CreateLayer("BackObject", ERenderObjectLayer::BackObject, ERenderListSort::Alpha);
	CRenderManager::GetInst()->CreateLayer("PlayerBackSubObject", ERenderObjectLayer::PlayerBackSubObject, ERenderListSort::Alpha);
	CRenderManager::GetInst()->CreateLayer("PlayerFrontSubObject", ERenderObjectLayer::PlayerFrontSubObject, ERenderListSort::Alpha);
	CRenderManager::GetInst()->CreateLayer("FrontObject", ERenderObjectLayer::FrontObject, ERenderListSort::Alpha);
	for (int32 i = 0; i < MAX_FRONT_TILE_RENDER_LAYER; ++i)
	{
		CRenderManager::GetInst()->CreateLayer("FrontTile" + std::to_string(i), ERenderObjectLayer::Count + i, ERenderListSort::Alpha);
	}
	for (int32 i = 0; i < MAX_FORE_GROUND_RENDER_LAYER; ++i)
	{
		CRenderManager::GetInst()->CreateLayer("Foreground" + std::to_string(i), ERenderObjectLayer::Count + MAX_FRONT_TILE_RENDER_LAYER + i, ERenderListSort::Alpha);
	}
	CRenderManager::GetInst()->CreateLayer("WorldWidget", ERenderObjectLayer::Count + MAX_FRONT_TILE_RENDER_LAYER + MAX_FORE_GROUND_RENDER_LAYER, ERenderListSort::Alpha);
}

bool CGlobalSetting::InitMouseWidget()
{
	// 마우스 위젯 생성
	//auto MouseWidget = CRenderManager::GetInst()->SetMouseWidget<CMouseWidget>(
	//	EMouseState::Normal, "MouseNormal").lock();

	//std::vector<const TCHAR*>	TextureFileName;

	//for (int i = 0; i <= 12; ++i)
	//{
	//	//TCHAR	FileName[MAX_PATH] = {};
	//	TCHAR* FileName = new TCHAR[MAX_PATH];
	//	memset(FileName, 0, sizeof(TCHAR) * MAX_PATH);
	//	wsprintf(FileName,
	//		TEXT("Mouse/Default/%d.png"), i);
	//	TextureFileName.push_back(FileName);
	//}

	//MouseWidget->SetSize(32.f, 31.f);
	//MouseWidget->SetTexture("MouseNormal", TextureFileName);

	//MouseWidget->AddBrushFrame(0.f, 0.f, 32.f, 31.f, 13);
	//MouseWidget->SetBrushAnimation(true);

	//for (int i = 0; i <= 12; ++i)
	//{
	//	delete[] TextureFileName[i];
	//}
	//TextureFileName.clear();

	//auto	ShaderMgr = CAssetManager::GetInst()->GetShaderManager().lock();

	//ShaderMgr->CreateShader<CShaderPostProcessHit>("Hit");

	return true;
}
