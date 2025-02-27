#include "pch.h"
#include "SpaceSystem.h"

#include "InputManager.h"
#include "Space.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "PlayerInformation.h"

void SpaceSystem::Configure(World* world)
{
	world->Subscribe<Events::SpaceAssemble>(this);
	world->Subscribe<Events::SpaceReturn>(this);
}

void SpaceSystem::Deconfigure(World* world)
{
	world->UnsubscribeAll(this);
}

void SpaceSystem::Receive(World* world, const Events::SpaceAssemble& event)
{
	Entity* objectEntity;
	Entity* subjectEntity;

	world->each<Space>([&](Entity* ent, ComponentHandle<Space> space)->void
		{
			if (space->m_SpaceIndex == event.objectIndex)
				objectEntity = ent;
			else if (space->m_SpaceIndex == event.subjectIndex)
				subjectEntity = ent;
		});

	const auto object = objectEntity->get<Space>();
	const auto subject = subjectEntity->get<Space>();
	Vector3D& subjectDistance = subject->m_Exits[event.subjectExit].m_Distance;
	const Vector3D objectDistance = object->m_Exits[event.objectExit].m_Distance;
	
	const int rotationKey = object->m_Exits[event.objectExit].m_ExitDirection - subject->m_Exits[event.subjectExit].m_ExitDirection;

	if (rotationKey == -1 || rotationKey == 3)
	{
		subjectEntity->get<Transform>()->m_Rotation.AddY(270.f);
		for(auto& it : subject->m_Exits)
		{
			it.m_ExitDirection = (it.m_ExitDirection + 1) % 4;
			it.m_Distance = Vector3D{ -it.m_Distance.GetZ(), it.m_Distance.GetY(), it.m_Distance.GetX() };
		}
	}
	else if (rotationKey == 1 || rotationKey == -3)
	{
		subjectEntity->get<Transform>()->m_Rotation.AddY(90.f);
		for (auto& it : subject->m_Exits)
		{
			it.m_ExitDirection = (it.m_ExitDirection + 3) % 4;
			it.m_Distance = Vector3D{ it.m_Distance.GetZ(), it.m_Distance.GetY(), -it.m_Distance.GetX() };
		}
	}
	else if (rotationKey == 0)
	{
		subjectEntity->get<Transform>()->m_Rotation.AddY(180.f);
		for (auto& it : subject->m_Exits)
		{
			it.m_ExitDirection = (it.m_ExitDirection + 2) % 4;
			it.m_Distance = Vector3D{ -it.m_Distance.GetX(), it.m_Distance.GetY(), -it.m_Distance.GetZ() };
		}
	}

	const Vector3D vec = objectDistance - subjectDistance;

	if (subjectEntity->get<Transform>()->m_Position == objectEntity->get<Transform>()->m_Position + vec)
		return;

	subjectEntity->get<Transform>()->m_Position.SetY(0);
	subjectEntity->get<Transform>()->m_Position = objectEntity->get<Transform>()->m_Position + vec;
}

void SpaceSystem::Receive(World* world, const Events::SpaceReturn& event)
{
	world->each<Space, Transform>([&](Entity* ent, ComponentHandle<Space> space, ComponentHandle<Transform> transform)
		{
			if (event.spaceIndex == space->m_SpaceIndex)
			{
				transform->m_Position.SetY(10000.f);
			}
		});
}

void SpaceSystem::Tick(World* world, ECS::DefaultTickData data)
{
	//return all
	if (InputM->GetKeyDown(Key::F12))
	{
		world->each<Space, Transform>([&](Entity* ent, ComponentHandle<Space> space, ComponentHandle<Transform> transform)->void
			{
				if (!space->m_IsPlayerExist)
					transform->m_Position.SetY(10000.f * space->m_SpaceIndex);
			});

	}

	ComponentHandle<PlayerInformation> information;
	world->each<PlayerInformation>([&](Entity* ent_player, ComponentHandle<PlayerInformation> component)->void
		{
			information = component;
		});

	//std::vector<Entity*> _entity;
	world->each<BoxCollider, Space>([&](Entity* ent, ComponentHandle<BoxCollider> collider, ComponentHandle<Space> sp)->void
		{
			ComponentHandle<Space> space;
			if (ent->getParent())
			{
				if (ent->getParent()->get<Space>())
				{
					space = ent->getParent()->get<Space>();
				}
			}

			if (collider->m_State == CollisionState::ENTER)
			{
				if (space)
				{
					space->m_IsPlayerExist = true;
					information->m_VisitedRooms.push(space->m_SpaceIndex);

					/*if (space->m_IsPlayerExist == true)
					{
						space->m_IsPlayerExist = false;
						_entity.push_back(ent->getParent());
					}
					else
					{
						collider->m_State = CollisionState::EXIT;
						space->m_IsPlayerExist = true;
					}*/
				}
			}
			/*else if (collider->m_State == CollisionState::EXIT)
			{
				if(space)
					space->m_IsPlayerExist = false;
			}*/
		});

	//world->each<PlayerInformation>([&](Entity* ent_player, ComponentHandle<PlayerInformation> component)->void
	//	{
	//		if (!_entity.empty())
	//		{
	//			for (auto& ent : _entity)
	//			{
	//				
	//					if (ent->has<Space>())
	//					{
	//						component->m_VisitedRooms.push(ent->get<Space>()->m_SpaceIndex);
	//						std::cout << "Player visit Room : " << ent->get<Space>()->m_SpaceIndex << std::endl;
	//					}
	//			}
	//		}
	//		

	//	});

	//_entity.clear();
}

