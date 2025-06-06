#pragma once
#include "DynamicCollider.h"
#include "InputManager.h"
#include "PhysicsManager.h"
#include "Movement.h"
#include "Script.h"
#include "Transform.h"
#include "Space.h"

#include <queue>

class PlayerScript : public Script
{
public:
	PlayerScript(Entity* ent)
		: Script(ent)
	{}
	virtual ~PlayerScript() override = default;
	queue<int> m_VisitedRooms;

	virtual void Awake() override
	{
		m_pOwner->get<Transform>()->m_FreezeRotationX = true;
		m_pOwner->get<Transform>()->m_FreezeRotationZ = true;

		//Todo ����
		//PhysicsManager::GetInstance()->GetDynamicCollider(m_pOwner->getEntityId())->FreezeRotation(true,true,true);

		InputM->SetCameraMode(true);

		//m_pOwner->getWorld()->emit<Events::BroadCastPlayer>({ m_pOwner });
	}

	virtual void Update(float deltaTime) override
	{
		if (m_pOwner->has<Movement>())
		{
			if(InputM->GetKey(Key::W))
			{
				m_pOwner->get<Movement>()->m_CurrentMoveState += MoveState::FRONT;
			}
			if (InputM->GetKey(Key::A))
			{
				m_pOwner->get<Movement>()->m_CurrentMoveState += MoveState::LEFTWARD;
			}
			if (InputM->GetKey(Key::D))
			{
				m_pOwner->get<Movement>()->m_CurrentMoveState += MoveState::RIGHTWARD;
			}
			if (InputM->GetKey(Key::S))
			{
				m_pOwner->get<Movement>()->m_CurrentMoveState += MoveState::BACK;
			}
			if (InputM->GetKey(Key::F1))
			{
				m_pOwner->getWorld()->each<Space>([&](Entity* ent, ComponentHandle<Space> space)
					{
						if (space->m_SpaceIndex == 13)
							ent->get<Transform>()->m_Position = Vector3D{ 0.f,0.f,0.f };
					});
			}
			if (InputM->GetKey(Key::F2))
			{
				m_pOwner->getWorld()->each<Space>([&](Entity* ent, ComponentHandle<Space> space)
					{
						if (space->m_SpaceIndex == 13)
							ent->get<Transform>()->m_Position = Vector3D{ 0.f,0.f,0.f };
					});
			}
			if (InputM->GetKey(Key::F3))
			{
				m_pOwner->getWorld()->each<Space>([&](Entity* ent, ComponentHandle<Space> space)
					{
						if (space->m_SpaceIndex == 13)
							ent->get<Transform>()->m_Position = Vector3D{ 0.f,0.f,0.f };
					});
			}
			if (InputM->GetKey(Key::F4))
			{
				m_pOwner->getWorld()->each<Space>([&](Entity* ent, ComponentHandle<Space> space)
					{
						if (space->m_SpaceIndex == 13)
							ent->get<Transform>()->m_Position = Vector3D{ 0.f,0.f,0.f };
					}); m_pOwner->get<Movement>()->m_CurrentMoveState += MoveState::BACK;
			}

			m_pOwner->get<Movement>()->m_CurrentRotation[0] = InputM->GetMouseMove().x;
		}

	}
};