#include "Pch.h"
#include "DialogueMovement.h"

#include "Component/SceneComponent.h"
#include "Component/CharacterMovementComponent.h"

#include "Movement/PlayerGameState.h"

#include "World/World.h"
#include "World/WorldUIManager.h"
#include "UI/Main/DialogueWidget.h"

#include "Character/Character.h"
#include "Character/Player/Player.h"
#include "Dialogue/Conversable.h"

void CDialogueMovement::Init(std::weak_ptr<CCharacterMovementComponent> Owner)
{
	CCharacterMovement::Init(Owner);

	std::shared_ptr<CWorld> World = Owner.lock()->GetWorld().lock();
	std::shared_ptr<CWorldUIManager> UIMgr = World->GetUIManager().lock();
	
	mWidget = UIMgr->CreateWidget<CDialogueWidget>("DialogueWidget").lock();
}

bool CDialogueMovement::CanMove() const
{
	if (CCharacterMovement::CanMove() == false)
	{
		return false;
	}

	FPlayerGameState* SharedState = static_cast<FPlayerGameState*>(GetSharedGameState());
	if (SharedState == nullptr)
	{
		return false;
	}

	std::shared_ptr<IConversable> ConversableObject = SharedState->GetConversableObject().lock();
	if (ConversableObject == nullptr || ConversableObject->CanStartDialogue() == false)
	{
		return false;
	}
	return true;
}

EMovementResult CDialogueMovement::Move(const void* Payload)
{
	CCharacterMovement::Move(Payload);

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	std::shared_ptr<CCharacter> OwnerChar = GetOwnerCharacter();
	FPlayerGameState* SharedState = static_cast<FPlayerGameState*>(GetSharedGameState());
	if (OwnerComponent == nullptr || OwnerChar == nullptr || SharedState == nullptr)
	{
		return EMovementResult::LooseStack;
	}
	std::shared_ptr<IConversable> ConversableObject = SharedState->GetConversableObject().lock();
	if (ConversableObject == nullptr)
	{
		return EMovementResult::LooseStack;
	}
	OwnerComponent->SetApplyMove(false);

	mWidget->mOnEndDialogue = [this]() {
		mNeedToStop = true;
		mAccDelayTime = 0.f;
		};
	mWidget->StartDialogue(OwnerChar, ConversableObject);

	return EMovementResult::Ongoing;
}

EMovementResult CDialogueMovement::Update(float DeltaTime)
{
	CCharacterMovement::Update(DeltaTime);

	if (mNeedToStop == true)
	{
		mAccDelayTime += DeltaTime;
		if (mAccDelayTime >= mEndDelayTime)
		{
			mNeedToStop = false;
			return EMovementResult::Abort;
		}
	}

	return EMovementResult::Ongoing;
}

void CDialogueMovement::Stop()
{
	CCharacterMovement::Stop();

	std::shared_ptr<CCharacterMovementComponent> OwnerComponent = GetOwnerComponent();
	if (OwnerComponent == nullptr)
	{
		return;
	}
	OwnerComponent->SetApplyMove(true);
}
