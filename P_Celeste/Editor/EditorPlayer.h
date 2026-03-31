#pragma once

#include "Object/GameObject.h"

class CInput;

class CCameraComponent;
class CObjectMovementComponent;
class CEditorWidget;

class CEditorPlayer : public CGameObject
{
	friend class CWorld;
	friend class CObject;

protected:
	CEditorPlayer();
	CEditorPlayer(const CEditorPlayer& ref) = default;
	CEditorPlayer(CEditorPlayer&& ref) noexcept = default;

public:
	virtual ~CEditorPlayer() override = default;

public:
	virtual void Begin();
	virtual bool Init();

private:
	void MoveForward();
	void MoveBack();
	void MoveLeft();
	void MoveRight();

	void MoveUp();
	void MoveDown();

	void MoveClick();
	void MoveCopyClick();
	void MovePasteClick();

private:
	void MoveTileFrameUp();
	void MoveTileFrameDown();

	void MoveTileOnOff();
	void MoveTileLock();

private:
	virtual void BindPlayerInputs(std::shared_ptr<CInput>& InputManager);

private:
	std::weak_ptr<CCameraComponent> mCameraComponent;
	std::weak_ptr<CObjectMovementComponent> mMovementComponent;

private:
	std::weak_ptr<CEditorWidget> mEditorWidget;
};

