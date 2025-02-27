#pragma once
#pragma once
#include "Camera.h"
#include "ECS.h"
#include "InputManager.h"
#include "Script.h"
#include "EntityIdentifier.h"
#include "Sprite2D.h"
#include "UI.h"
#include "Clue.h"
static bool m_IsShowClue = false;

class ClueSpriteScript : public Script
{
public:

	explicit ClueSpriteScript(Entity* ent)
		:Script(ent)
	{
	}

	virtual ~ClueSpriteScript() = default;

	virtual void Update(float delatTime) override
	{

		if (m_pOwner->get<EntityIdentifier>()->m_EntityName == "ClueNoteSprite")
		{
			if (InputManager::GetInstance()->GetKeyDown(Key::TAB))
			{
				m_IsShowClue = !m_IsShowClue;

				m_pOwner->get<Sprite2D>()->m_IsRendered = m_IsShowClue;
				m_pOwner->get<Clue>()->m_IsBookOpened = m_IsShowClue;
			}
			/*if (m_IsShowClue)
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = true;a
				m_IsBookOpened = true;
			}
			else
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = false;
				m_IsBookOpened = false;
			}*/
		}
		else if (m_pOwner->get<EntityIdentifier>()->m_EntityName == "FrontDoorSprite")
		{
			if (m_IsShowClue)
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = true;
			}
			else
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = false;
			}
			auto uiState = m_pOwner->get<UI>()->m_UIstate;
			switch (uiState)
			{
			case CLICK:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_01.png";
				m_pOwner->getWorld()->emit<Events::CluePageChange>({ 1 });

				break;
			case HOLD:

				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_click_btn_01.png";
				break;
			case UINONE:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_btn_01.png";
				break;
			case HOVER:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_01.png";
				break;
			default:
				break;
			}
		}
		else if (m_pOwner->get<EntityIdentifier>()->m_EntityName == "LivingRoomSprite")
		{
			if (m_IsShowClue)
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = true;
			}
			else
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = false;
			}

			auto uiState = m_pOwner->get<UI>()->m_UIstate;
			switch (uiState)
			{
			case CLICK:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_02.png";
				m_pOwner->getWorld()->emit<Events::CluePageChange>({ 2 });
				break;
			case HOLD:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_click_btn_02.png";
				break;
			case UINONE:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_btn_02.png";
				break;
			case HOVER:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_02.png";
				break;
			default:
				break;
			}
		}
		else if (m_pOwner->get<EntityIdentifier>()->m_EntityName == "BookRoomSprite")
		{
			if (m_IsShowClue)
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = true;
			}
			else
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = false;
			}

			auto uiState = m_pOwner->get<UI>()->m_UIstate;
			switch (uiState)
			{
			case CLICK:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_03.png";
				m_pOwner->getWorld()->emit<Events::CluePageChange>({ 3 });
				break;
			case HOLD:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_click_btn_03.png";
				break;
			case UINONE:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_btn_03.png";
				break;
			case HOVER:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_03.png";
				break;
			default:
				break;
			}
		}
		else if (m_pOwner->get<EntityIdentifier>()->m_EntityName == "ItemStorageSprite")
		{
			if (m_IsShowClue)
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = true;
			}
			else
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = false;
			}

			auto uiState = m_pOwner->get<UI>()->m_UIstate;
			switch (uiState)
			{
			case CLICK:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_04.png";
				m_pOwner->getWorld()->emit<Events::CluePageChange>({ 4 });
				break;
			case HOLD:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_click_btn_04.png";
				break;
			case UINONE:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_btn_04.png";
				break;
			case HOVER:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_04.png";
				break;
			default:
				break;
			}
		}
		else if (m_pOwner->get<EntityIdentifier>()->m_EntityName == "LastRoomSprite")
		{
			if (m_IsShowClue)
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = true;
			}
			else
			{
				m_pOwner->get<Sprite2D>()->m_IsRendered = false;
			}

			auto uiState = m_pOwner->get<UI>()->m_UIstate;
			switch (uiState)
			{
			case CLICK:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_05.png";
				m_pOwner->getWorld()->emit<Events::CluePageChange>({ 5 });
				break;
			case HOLD:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_click_btn_05.png";
				break;
			case UINONE:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_btn_05.png";
				break;
			case HOVER:
				m_pOwner->get<Sprite2D>()->m_FileName = "../Resource/UI/cluenote_mouseup_btn_05.png";
				break;
			default:
				break;
			}
		}

		if (m_IsShowClue)
		{
			InputManager::GetInstance()->LockKey(Key::W);
			InputManager::GetInstance()->LockKey(Key::A);
			InputManager::GetInstance()->LockKey(Key::S);
			InputManager::GetInstance()->LockKey(Key::D);
			InputManager::GetInstance()->LockKey(Key::D);
			InputManager::GetInstance()->LockKey(Key::E);
			InputManager::GetInstance()->LockKey(Key::ESC);
		}
		else
		{
			InputManager::GetInstance()->UnlockKey();
		}
	}

};