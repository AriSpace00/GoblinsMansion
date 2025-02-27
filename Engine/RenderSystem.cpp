#include "pch.h"
#include "ECS.h"
#include "RenderSystem.h"
#include "StaticMesh.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "EntityIdentifier.h"
#include "Light.h"
#include "RenderManager.h"

using namespace ECS;

void RenderSystem::Configure(ECS::World* world)
{
	world->Subscribe<ECS::Events::OnComponentAssigned<StaticMesh>>(this);
	world->Subscribe<ECS::Events::OnComponentAssigned<Light>>(this);
	world->Subscribe<ECS::Events::OnEntityDestroyed>(this);
	world->Subscribe<ECS::Events::OnComponentAssigned<DynamicText>>(this);
}

void RenderSystem::Deconfigure(ECS::World* world)
{
	world->UnsubscribeAll(this);
}

void RenderSystem::Tick(ECS::World* world, ECS::DefaultTickData data)
{
	world->each<StaticMesh, Transform>([&](Entity* entity, const ComponentHandle<StaticMesh> staticMesh, ComponentHandle<Transform> transform)->void
		{
			if (staticMesh->m_FileName != "")
			{
				staticMesh->m_BoundingBox.Center = Vector3(transform->m_WorldMatrix.m_41, transform->m_WorldMatrix.m_42 + staticMesh->m_BoundingBox.Extents.y / 2, transform->m_WorldMatrix.m_43);

				if (staticMesh->m_IsModelCreated)
				{

					if (RenderManager::GetInstance()->Culling(staticMesh->m_BoundingBox))
						RenderManager::GetInstance()->AddStaticMesh(staticMesh->m_FileName, transform->m_WorldMatrix.ConvertToMatrix());
					//RenderManager::GetInstance()->AddBoundingBox(staticMesh->m_BoundingBox);
				}
				else
				{
					RenderManager::GetInstance()->CreateModel(staticMesh->m_FileName, staticMesh->m_BoundingBox);
					staticMesh->m_IsModelCreated = true;
				}
			}

		});

	//world->each<Transform, BoxCollider>([&](Entity* entity, ComponentHandle<Transform> transform, ComponentHandle<BoxCollider> boxCollider)->void
	//	{
	//		//RenderManager::GetInstance()->AddStaticMesh(staticMesh->m_FileName, transform->m_WorldMatrix.ConvertToMatrix());
	//		RenderManager::GetInstance()->AddColliderBox(boxCollider->m_WorldPosition, boxCollider->m_Size, transform->m_Rotation);
	//	});

	world->each<Light, Transform>([&](Entity* ent, ComponentHandle<Light> light, ComponentHandle<Transform> transform)->void
		{
			if (light->m_Type == LightType::Point)
			{
				RenderManager::GetInstance()->EditPointLight(ent->getEntityId(), transform->m_WorldMatrix.ConvertToMatrix().Translation(), light->m_Color, light->m_Intensity, light->m_Radius);
			}
			else
			{
				RenderManager::GetInstance()->EditDirectionalLight(light->m_Direction.ConvertToVector3(), light->m_Color.ConvertToVector3());
			}
		});

	world->each<BoxCollider, DynamicText>([&](Entity* ent, ComponentHandle<BoxCollider> boxCollider, ComponentHandle<DynamicText> dynamicText)->void
		{
			if (boxCollider->m_IsRaycastHit)
			{
				RenderManager::GetInstance()->EditDynamicText(ent->getEntityId(), dynamicText->m_CurrentTextIndex, dynamicText->m_FontIndex, dynamicText->m_IsTextShow);
			}
			else
			{
				RenderManager::GetInstance()->EditDynamicText(ent->getEntityId(), dynamicText->m_CurrentTextIndex, dynamicText->m_FontIndex, dynamicText->m_IsTextShow);
			}
		});
}

void RenderSystem::Receive(ECS::World* world, const ECS::Events::OnComponentAssigned<StaticMesh>& event)
{
	// minjeong : fbx load test
	if (event.component->m_FileName != "")
	{
		RenderManager::GetInstance()->CreateModel(event.component->m_FileName, event.component->m_BoundingBox);
		event.component->m_IsModelCreated = true;
	}
}

void RenderSystem::Receive(ECS::World* world, const ECS::Events::OnComponentAssigned<Light>& event)
{
	auto pos = event.entity->get<Transform>()->m_WorldMatrix.ConvertToMatrix().Translation();

	if (event.component->m_Type == LightType::Point)
	{
		RenderManager::GetInstance()->CreatePointLight(event.entity->getEntityId(), pos, event.component->m_Color, event.component->m_Intensity, event.component->m_Radius);
	}
}

void RenderSystem::Receive(ECS::World* world, const ECS::Events::OnEntityDestroyed& event)
{
	if (event.entity->has<Light>())
	{
		if (event.entity->get<Light>()->m_Type == LightType::Point)
		{
			RenderManager::GetInstance()->DeletePointLight(event.entity->getEntityId());
		}
	}
	if (event.entity->has<DynamicText>())
	{
		RenderManager::GetInstance()->DeleteDynamicText(event.entity->getEntityId());
	}
}

void RenderSystem::Receive(ECS::World* world, const ECS::Events::OnComponentAssigned<DynamicText>& event)
{
	world->each<DynamicText>([&](Entity* entity, ComponentHandle<DynamicText> dynamicText)->void
		{
			RenderManager::GetInstance()->AddDynamicText(entity->getEntityId(), dynamicText->m_Text[event.component->m_TextNum], dynamicText->m_FontIndex);
		});
}
