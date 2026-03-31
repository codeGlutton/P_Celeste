#pragma once

#include "Object/GameObject.h"
#include "Dialogue/Conversable.h"

class CInput;

class CCollider;
class CColliderBox2D;
class CMeshComponent;
class CAnimation2DComponent;
class CCharacterMovementComponent;

class CAnimStateMachine;

class CCharacter : public CGameObject, public IConversable
{
	friend class CWorld;
	friend class CObject;

protected:
	CCharacter();
	CCharacter(const CCharacter& ref) = default;
	CCharacter(CCharacter&& ref) noexcept = default;

public:
	virtual ~CCharacter() override;

public:
	virtual bool Init() override;
	virtual void Update(float DeltaTime) override;
	virtual void PostUpdate(float DeltaTime) override;

private:
	virtual std::shared_ptr<CCollider> MakeRootCollider() = 0;
	virtual void BindPlayerInputs(std::shared_ptr<CInput>& InputManager)
	{
	}
	virtual void ConfigStateMachine(std::shared_ptr<CAnimStateMachine>& StateMachine)
	{
	}

public:
	virtual std::shared_ptr<CGameObject> GetConversableObject() override
	{
		return mSelf.lock();
	}
	virtual std::shared_ptr<FDialoguePanelBrush> GetDialoguePanelBrush() override
	{
		return nullptr;
	}
	virtual std::shared_ptr<FDialoguePortraitBrush> GetDialoguePortraitBrush(const std::string& EmotionName) override
	{
		return nullptr;
	}
	virtual std::shared_ptr<FDialogueContext> GetDialogueContext() override
	{
		return nullptr;
	}

public:
	const FVector3& GetSightDir();
	void SetSightDir(const FVector3& SightDir);

public:
	std::shared_ptr<CCharacterMovementComponent>& GetMovementComp()
	{
		return mMovementComponent;
	}

protected:
	std::vector<std::string> GetEmotionTextureNames(const std::string& EmotionName, int32 Count, int32 StartOffset = 0);
	std::vector<std::pair<std::string, int32>> GetEmotionSoundNames(const std::string& EmotionName, int32 SampleCount, int32 Count, int32 StartOffset = 0);

protected:
	std::shared_ptr<CCollider> mBodyComponent;

protected:
	std::shared_ptr<CMeshComponent> mMeshComponent;
	std::shared_ptr<CAnimation2DComponent> mAnimation2DComponent;
	std::shared_ptr<CCharacterMovementComponent> mMovementComponent;

protected:
	std::shared_ptr<CAnimStateMachine> mAnimStateMachine;

protected:
	bool mAutoSymmetryToLastInput = true;
};

