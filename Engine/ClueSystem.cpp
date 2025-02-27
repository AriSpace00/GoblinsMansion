#include "pch.h"
#include "ClueSystem.h"
#include "EntityIdentifier.h"
#include "Clue.h"
#include "InputManager.h"
#include "Sprite2D.h"
#include "ClueSpriteScript.h"
void ClueSystem::Configure(World* world)
{
	world->Subscribe<Events::CluePageChange>(this);
	world->Subscribe<Events::ClueFound>(this);
}

void ClueSystem::Deconfigure(World* world)
{
	world->UnsubscribeAll(this);
}

void ClueSystem::Receive(World* world, const Events::CluePageChange& event)
{
	m_CurrentPage = event.page;
}

void ClueSystem::Receive(World* world, const Events::ClueFound& event)
{
 	world->each<Clue, Sprite2D>([&](Entity* ent, ComponentHandle<Clue> clue, ComponentHandle<Sprite2D> sprite)
		{
			if ((clue->m_Page == event.page && clue->m_Index == event.index) && clue->m_Page != 0 && clue->m_Index != 0)
				clue->m_render = true;
		});
}

void ClueSystem::Tick(World* world, ECS::DefaultTickData data)
{
	world->each<Clue, Sprite2D>([&](Entity* ent, ComponentHandle<Clue> clue, ComponentHandle<Sprite2D> sprite)
		{
			if (ent->get<EntityIdentifier>()->m_EntityName == "ClueNoteSprite")
			{
				m_IsClueOn = clue->m_IsBookOpened;
			}

			if (ent->get<EntityIdentifier>()->m_EntityName != "ClueNoteSprite")
			{
				if (m_IsClueOn && clue->m_render && m_CurrentPage == clue->m_Page)
				{
					sprite->m_IsRendered = true;
				}
				else
				{
					sprite->m_IsRendered = false;
				}
			}
		});
}