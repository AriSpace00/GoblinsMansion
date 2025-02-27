#pragma once
#include "pch.h"
#include "DynamicText.h"
#include "ECS.h"
#include "InputManager.h"
#include "Script.h"
#include "BoxCollider.h"
#include "PlayerInformation.h"
#include "Transform.h"

struct DynamicTextScript : public Script
{
	DynamicTextScript(Entity* ent)
		:Script(ent)
	{}
	virtual ~DynamicTextScript() override = default;
	virtual void Awake() override
	{
		if(m_pOwner->has<DynamicText>())
		{
			m_Size = m_pOwner->get<DynamicText>()->m_Text[m_pOwner->get<DynamicText>()->m_TextNum].size();
			m_pOwner->get<DynamicText>()->m_CurrentTextIndex = 0;
		}
		m_pOwner->getWorld()->each<PlayerInformation>([&](Entity* ent, ComponentHandle<PlayerInformation> info)
			{
				m_Player = ent;
			});
	}

	virtual void Update(float deltaTime) override
	{
		if(InputM->GetKeyDown(Key::SPACE))
		{
			if (m_pOwner->get<DynamicText>()->m_IsTextShow)
			{
				m_pOwner->get<DynamicText>()->m_CurrentTextIndex += 1;
				m_pOwner->get<DynamicText>()->m_CurrentTextIndex %= m_Size;
				m_pOwner->getWorld()->emit<Events::DynamicTextChange>({ m_pOwner });
			}
		}

		const Vector3 playerPos = m_Player->get<Transform>()->m_WorldMatrix.ConvertToMatrix().Translation();
		const Vector3 objectPos = m_pOwner->get<Transform>()->m_WorldMatrix.ConvertToMatrix().Translation();

		if (m_pOwner->has<BoxCollider>())
		{
			if (m_pOwner->get<BoxCollider>()->m_WasRaycastHit != (m_pOwner->get<BoxCollider>()->m_IsRaycastHit))
			{
				m_pOwner->get<DynamicText>()->m_IsTextShow = (m_pOwner->get<BoxCollider>()->m_IsRaycastHit);
				if(!m_pOwner->get<DynamicText>()->m_IsTextShow)
					m_pOwner->get<DynamicText>()->m_CurrentTextIndex = 0;
				m_pOwner->getWorld()->emit<Events::DynamicTextChange>({ m_pOwner });
				m_pOwner->getWorld()->emit<Events::ClueFound>({ m_pOwner->get<DynamicText>()->m_Page, m_pOwner->get<DynamicText>()->m_Index });
			}
		}
	}

	int m_Size = 0;
	Entity* m_Player = nullptr;
};
