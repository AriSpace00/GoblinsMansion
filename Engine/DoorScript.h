#pragma once
#include "BoxCollider.h"
#include "ECS.h"
#include "InputManager.h"
#include "Interactive.h"
#include "Script.h"
#include "Transform.h"

struct DoorScript : public Script
{
	DoorScript(Entity* ent)
		:Script(ent)
	{}
	virtual ~DoorScript() = default;

	void Awake() override
	{
		m_pOwner->getWorld()->each<PlayerInformation>([&](Entity* ent,const ComponentHandle<PlayerInformation> playerInfo)
			{
				m_PlayerInfo = playerInfo;
			});
	}

	void Update(float deltaTime) override
	{
		bool m_IsInteract = m_pOwner->get<Interactive>()->m_IsInteract;
		if (m_pOwner->has<BoxCollider>())
		{
			if (m_pOwner->get<BoxCollider>()->m_IsRaycastHit)
			{
				// 외곽선 처리
				if (InputM->GetKeyDown(Key::E) && m_IsDoorActive == false)		// 상호작용 중에는 키를 인식하지 않는다.
				{
					if (m_pOwner->get<Interactive>()->m_DoorIndex == 1)
						m_PlayerInfo->m_InteractingCount++;
					if (!m_IsInteract)
					{
						m_pOwner->getWorld()->each<Interactive>([&](Entity* ent, ComponentHandle<Interactive> interactive)
							{
								if(interactive->m_DoorIndex == m_pOwner->get<Interactive>()->m_DoorIndex)
									interactive->m_IsInteract = !m_IsInteract;
							});						
					}

					m_pOwner->getWorld()->each<Interactive>([&](Entity* ent, ComponentHandle<Interactive> interactive)
						{
							if (interactive->m_DoorIndex == m_pOwner->get<Interactive>()->m_DoorIndex)
								interactive->m_IsInteract = !m_IsInteract;

						});

					m_IsDoorActive = true;
				}
				
				// 상호작용 UI 출력
				if (m_pOwner->has<Sprite2D>())
				{
					if (m_IsDoorActive == false)
					{
						m_pOwner->get<Sprite2D>().get().m_IsRendered = true;
					}
					// 문이 열리는 중에는 상호작용 UI 를 출력하지 않는다.
					else if (m_IsDoorActive == true)
					{
						m_pOwner->get<Sprite2D>().get().m_IsRendered = false;
					}
				}

			}
			else
			{
				if (m_pOwner->has<Sprite2D>())
					m_pOwner->get<Sprite2D>().get().m_IsRendered = false;
			}

		}

		
		if(m_pOwner->has<Interactive>())
		{
			// 문이 동작중이지 않은 상태에서만 문을 움직인다.
			if (m_pOwner->get<Interactive>()->m_IsInteract)
			{
				// 문이 다 열렸다. (상호작용 끝)
				if (m_pOwner->get<Transform>()->m_Position.GetX() >= 150.f || m_pOwner->get<Transform>()->m_Position.GetX() <= -150.f)
				{
					m_pOwner->get<Transform>()->m_Position.SetX(150.f * m_pOwner->get<Interactive>()->m_OpeningDir);
					m_IsDoorActive = false;
					if(m_pOwner->get<Interactive>()->m_DoorIndex == 1)
					{
						m_PlayerInfo->m_Room6Check = false;
					}
				}
				else
				{
					m_pOwner->get<Transform>()->m_Position.AddX(m_pOwner->get<Interactive>()->m_OpeningDir * OBJECT_SPEED * deltaTime);
					m_IsDoorActive = true;
				}
			}
			else
			{
				// 문이 다 닫혔다. (상호작용 끝)
				if (m_pOwner->get<Transform>()->m_Position.GetX() <= 50.f && m_pOwner->get<Transform>()->m_Position.GetX() >= -50.f)
				{
					m_pOwner->get<Transform>()->m_Position.SetX(50.f * m_pOwner->get<Interactive>()->m_OpeningDir);
					m_IsDoorActive = false;
					if (m_pOwner->get<Interactive>()->m_DoorIndex == 1)
					{
						m_PlayerInfo->m_Room6Check = true;
					}
				}
				else
				{
					m_pOwner->get<Transform>()->m_Position.AddX(-m_pOwner->get<Interactive>()->m_OpeningDir * OBJECT_SPEED * deltaTime);
					m_IsDoorActive = true;
				}
			}
		}

	}

private:
	bool m_IsDoorActive = false;
	ComponentHandle<PlayerInformation> m_PlayerInfo;
};