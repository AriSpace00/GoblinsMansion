#pragma once
#include "BoxCollider.h"
#include "InputManager.h"
#include "Interactive.h"
#include "Script.h"
#include "Transform.h"

struct DrawerScript : public Script
{
	DrawerScript(Entity* ent)
		:Script(ent)
	{
	}
	virtual ~DrawerScript() override = default;

	void Awake() override
	{
		m_pOwner->getWorld()->each<PlayerInformation>([&](Entity* ent, const ComponentHandle<PlayerInformation> playerInfo)
			{
				m_PlayerInfo = playerInfo;
			});
	}

	virtual void Update(float deltaTime) override
	{
		if(m_pOwner->has<BoxCollider>())
		{
			auto isRaycastHit = m_pOwner->get<BoxCollider>()->m_IsRaycastHit;
			if (isRaycastHit == true)
			{
				// 외곽선 처리
				if (InputM->GetKeyDown(Key::E) && m_IsDrawerActive == false)
				{
					m_pOwner->get<Interactive>()->m_IsInteract = !m_pOwner->get<Interactive>()->m_IsInteract;
					if(m_pOwner->get<Interactive>()->m_IsInteract)
					{
						m_PlayerInfo->m_Room7Check++;
					}
					else
					{
						m_PlayerInfo->m_Room7Check--;
					}
					m_IsDrawerActive = true;
				}

				// 상호작용 UI 출력
				if (m_pOwner->has<Sprite2D>())
				{
					if (m_IsDrawerActive == false)
					{
						m_pOwner->get<Sprite2D>().get().m_IsRendered = true;
					}
					else if (m_IsDrawerActive == true)
					{
						m_pOwner->get<Sprite2D>().get().m_IsRendered = false;
					}
				}

			}
			else if (isRaycastHit == false)
			{
				if (m_pOwner->has<Sprite2D>())
					m_pOwner->get<Sprite2D>().get().m_IsRendered = false;
			}
		}
		
		if (m_pOwner->has<Interactive>())
		{
			// 서랍이 동작 중이지 않은 상태에서만 문을 움직인다.
			if(m_pOwner->get<Interactive>()->m_IsInteract)
			{
				// 서랍이 다 열렸다. (상호작용 끝)
				if(m_pOwner->get<Transform>()->m_Position.GetZ() <= -35.f)
				{
					m_pOwner->get<Transform>()->m_Position.SetZ(-35.f);
					m_IsDrawerActive = false;
				}
				else
				{
					m_pOwner->get<Transform>()->m_Position.SetZ(m_pOwner->get<Transform>()->m_Position.GetZ() - OBJECT_SPEED * deltaTime);
					m_IsDrawerActive = true;
				}
			}
			else
			{
				// 서랍이 다 닫혔다. (상호작용 끝)
				if (m_pOwner->get<Transform>()->m_Position.GetZ() >= 0.f)
				{
					m_pOwner->get<Transform>()->m_Position.SetZ(0.f);
					m_IsDrawerActive = false;
				}
				else
				{
					m_pOwner->get<Transform>()->m_Position.SetZ(m_pOwner->get<Transform>()->m_Position.GetZ() + OBJECT_SPEED * deltaTime);
					m_IsDrawerActive = true;
				}
			}
		}
	}

private:
	bool m_IsDrawerActive = false;
	ComponentHandle<PlayerInformation> m_PlayerInfo;
};
