#include "Pch.h"
#include "LaughMovement.h"

#include "Component/SceneComponent.h"
#include "Component/WidgetComponent.h"

#include "World/World.h"
#include "World/WorldAssetManager.h"

EMovementResult CLaughMovement::Move(const void* Payload)
{
    CCharacterMovement::Move(Payload);

    std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
    if (UpdateComponent == nullptr)
    {
        return EMovementResult::LooseStack;
    }

    const std::shared_ptr<CWidgetComponent>* WidgetComp = static_cast<const std::shared_ptr<CWidgetComponent>*>(Payload);
    if (WidgetComp != nullptr)
    {
        mLaughWidgetComp = *WidgetComp;
        mLaughWidgetComp.lock()->GetWidget<CWidget>()->SetEnable(true);
    }

    UpdateComponent->GetWorld().lock()->GetWorldAssetManager().lock()->Sound3DPlay("S_OldLadyLaugh", UpdateComponent->GetWorldPos());

    return EMovementResult::Ongoing;
}

void CLaughMovement::Stop()
{
    CCharacterMovement::Stop();

    std::shared_ptr<CWidgetComponent> WidgetComp = mLaughWidgetComp.lock();
    if (WidgetComp != nullptr)
    {
        WidgetComp->GetWidget<CWidget>()->SetEnable(false);
        mLaughWidgetComp.reset();
    }

    std::shared_ptr<CSceneComponent> UpdateComponent = GetUpdateComponent();
    if (UpdateComponent != nullptr)
    {
        UpdateComponent->GetWorld().lock()->GetWorldAssetManager().lock()->SoundStop("S_OldLadyLaugh");
    }
}
