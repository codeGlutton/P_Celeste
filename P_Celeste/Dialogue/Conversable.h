#pragma once

class CGameObject;
struct FDialoguePanelBrush;
struct FDialoguePortraitBrush;
struct FDialogueContext;

class IConversable
{
public:
	virtual ~IConversable() = default;

public:
	virtual bool CanStartDialogue()
	{
		return false;
	}
	virtual std::shared_ptr<CGameObject> GetConversableObject() = 0;
	virtual std::shared_ptr<FDialoguePanelBrush> GetDialoguePanelBrush() = 0;
	virtual std::shared_ptr<FDialoguePortraitBrush> GetDialoguePortraitBrush(const std::string& EmotionName) = 0;
	virtual std::shared_ptr<FDialogueContext> GetDialogueContext() = 0;
};

