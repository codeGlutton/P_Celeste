
#include "Pch.h"
#include "Engine.h"
#include "resource.h"

#include "World/WorldManager.h"
#include "Render/RenderManager.h"

#include "Setting/GlobalSetting.h"
#include "World/TitleWorld.h"

#include "Character/Player/Player.h"

#include "Component/CharacterMovementComponent.h"

#include "GameObject/Item/CelesteRewardData.h"

#ifdef _DEBUG

#pragma comment(lib, "GameEngine_Debug.lib")

#else

#pragma comment(lib, "GameEngine.lib")

#endif // _DEBUG

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    CEngine::GetInst()->CreateEngineSetting<CGlobalSetting>();
    CRewardManager::GetInst()->MakeData<CCelesteRewardData>();

    if (CEngine::GetInst()->Init(hInstance, TEXT("GameClient"), IDI_ICON1, IDI_ICON1, 1024, 600, true) == false)
    {
        CEngine::DestroyInst();
        return 0;
    }

    {
        CEngine::CreateCDO<CPlayer>();
        CEngine::CreateCDO<CCharacterMovementComponent>();
        //CEngine::CreateCDO<CMonsterSpawnPoint>();
    }

    CRenderManager::GetInst()->SetDebugTarget(false);

    CWorldManager::GetInst()->CreateWorld<CTitleWorld>(false);
    int32 Ret = CEngine::GetInst()->Run();
    CEngine::DestroyInst();

    return Ret;
}
