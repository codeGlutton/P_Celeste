#include "Pch.h"
#include "AIFlyMovement.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

bool CAIFlyMovement::CanMove() const
{
    if (CCharacterMovement::CanMove() == false)
    {
        return false;
    }
    return true;
}

EMovementResult CAIFlyMovement::Move(const void* Payload)
{
    CCharacterMovement::Move(Payload);

    const FVector3* TargetPos = static_cast<const FVector3*>(Payload);
    std::shared_ptr<CSceneComponent> UpdateComp = GetUpdateComponent();
    if (TargetPos == nullptr || UpdateComp == nullptr)
    {
        return EMovementResult::LooseStack;
    }

    mMoveDir = *TargetPos - UpdateComp->GetWorldPos();
    if (mMoveDir.IsZero() == true)
    {
        return EMovementResult::LooseStack;
    }
    mDestPos = *TargetPos;
    mMoveDir.Normalize();

    for (auto& Event : mOnFly)
    {
        Event(mMoveDir);
    }

    return EMovementResult::Ongoing;
}

EMovementResult CAIFlyMovement::Update(float DeltaTime)
{
    CCharacterMovement::Update(DeltaTime);

    std::shared_ptr<CSceneComponent> UpdateComp = GetUpdateComponent();
    std::shared_ptr<CCharacterMovementComponent> OwnerComp = GetOwnerComponent();
    if (OwnerComp == nullptr || UpdateComp == nullptr)
    {
        return EMovementResult::Abort;
    }

    FVector3 Dir = mDestPos - UpdateComp->GetWorldPos();
    float DistSquared = Dir.LengthSquared();
    if (DistSquared <= mDistThreshold)
    {
        return EMovementResult::LooseStack;
    }

    if (mFollowTargetPos == true)
    {
        mMoveDir = Dir;
        mMoveDir.Normalize();
    }

    OwnerComp->AddMove(mMoveDir);
    OwnerComp->AddSight(mMoveDir);

    return EMovementResult::Ongoing;
}
