#include "pch.h"
#include "CollisionSystem.h"
#include "Space.h"
#include "EntityIdentifier.h"
#include "PhysicsManager.h"
#include "PlayerInformation.h"

void CollisionSystem::Configure(World* world)
{
	world->Subscribe<Events::OnComponentAssigned<BoxCollider>>(this);
	world->Subscribe<Events::ColliderEnter>(this);
	world->Subscribe<Events::ColliderExit>(this);
}

void CollisionSystem::Deconfigure(World* world)
{
	world->UnsubscribeAll(this);
}

void CollisionSystem::Receive(World* world, const Events::OnComponentAssigned<BoxCollider>& event)
{
	event.component->m_WorldPosition = event.entity->get<Transform>()->m_Position + event.component->m_Center;
	event.component->m_Rotation = event.entity->get<Transform>()->m_Rotation; // rotation test
	PhysicsManager::GetInstance()->CreateCollider(event.component.component, event.entity->getEntityId());
}

void CollisionSystem::Receive(World* world, const Events::ColliderEnter& event)
{
	Entity* entity = world->getById(event.entID);
	world->each<PlayerInformation>([&](Entity* ent, ComponentHandle<PlayerInformation> info)->void
		{
			info->m_CollidingEntities.push_back(entity->get<EntityIdentifier>()->m_EntityName);
		});
}

void CollisionSystem::Receive(World* world, const Events::ColliderExit& event)
{
	Entity* entity = world->getById(event.entID);
	world->each<PlayerInformation>([&](Entity* ent, ComponentHandle<PlayerInformation> info)->void
		{
			info->m_CollidingEntities.push_back(entity->get<EntityIdentifier>()->m_EntityName);
			erase_if(info->m_CollidingEntities, [&](const string& str)
				{
					return str == entity->get<EntityIdentifier>()->m_EntityName;
				});
		});
}

void CollisionSystem::Tick(World* world, ECS::DefaultTickData data)
{
	Entity* player = nullptr;
	ComponentHandle<PlayerInformation> info;

	world->each<PlayerInformation>([&](Entity* ent, ComponentHandle<PlayerInformation> playerInfo)->void
		{
			player = ent;
			info = playerInfo;
		});

	world->each<Transform, BoxCollider>([&](Entity* ent, ComponentHandle<Transform> transform, ComponentHandle<BoxCollider> collider)
		{
			collider->m_Rotation = transform->m_Rotation;
			if (collider->m_prevState == CollisionState::EXIT)
				collider->m_State = CollisionState::NONE;
			else if (collider->m_prevState == CollisionState::ENTER)
				collider->m_State = CollisionState::STAY;

			collider->m_prevState = collider->m_State;
			
			if (collider->m_IsRaycastHit)
			{
				if (player)
				{
					if (player->has<PlayerInformation>())
					{
						info->m_LookingEntity = ent->get<EntityIdentifier>()->m_EntityName;
						//std::cout << info->m_LookingEntity << std::endl;
					}
				}
			}
			collider->m_WasRaycastHit = collider->m_IsRaycastHit;
			collider->m_IsRaycastHit = false;

			/*if (info)
			{
				info->m_CollidingEntities = PhysicsManager::GetInstance()->GetCollisionObjects();
			}*/

			if (ent->get<EntityIdentifier>()->m_HasParent)
			{
				if (ent->m_parent->has<Transform>())
					collider->m_Rotation += ent->getParent()->get<Transform>()->m_Rotation;
			}

			if (collider->m_CollisionType == CollisionType::PLAYER)
			{
				XMVECTOR determinant;
				transform->m_Position = (DirectX::SimpleMath::Matrix::CreateTranslation(collider->m_WorldPosition.ConvertToVector3()) * XMMatrixInverse(&determinant, DirectX::SimpleMath::Matrix::CreateTranslation(collider->m_Center.ConvertToVector3()))).Translation();
			}
			else
				collider->m_WorldPosition = (DirectX::SimpleMath::Matrix::CreateTranslation(collider->m_Center.ConvertToVector3()) * transform->m_WorldMatrix.ConvertToMatrix()).Translation();

		});
}

