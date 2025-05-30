#include "pch.h"
#include "Prefab.h"

// Components Headers
#include "../Engine/Transform.h"
#include "../Engine/BoxCollider.h"
#include "../Engine/Camera.h"
#include "../Engine/Light.h"
#include "../Engine/EntityIdentifier.h"
#include "../Engine/Movement.h"
#include "../Engine/Script.h"
#include "../Engine/SampleScript.h"
#include "../Engine/StaticMesh.h"
#include "../Engine/Debug.h"
#include "../Engine/Sound.h"
#include "../Engine/Sprite2D.h"
#include "../Engine/RigidBody.h"
#include "../Engine/UI.h"
#include "../Engine/Space.h"
#include "../Engine/DynamicText.h"
#include "../Engine/PlayerInformation.h"
#include "../Engine/Interactive.h"
#include "../Engine/Clue.h"

// Script Headers
#include "../Engine/SampleScript.h"
#include "../Engine/FreeCameraScript.h"
#include "../Engine/PlayerScript.h"
#include "../Engine/POVCameraScript.h"
#include "../Engine/TestUIScript.h"
#include "../Engine/DynamicTextScript.h"
#include "../Engine/IntroCameraScript.h"
#include "../Engine/OutroScript.h"
#include "../Engine/DrawerScript.h"
#include "../Engine/IntroDoorScript.h"
#include "../Engine/DoorScript.h"
#include "../Engine/IntroButtonScript.h"
#include "../Engine/PauseScript.h"
#include "../Engine/ClueSpriteScript.h"

#include "../Engine/FreeCameraScript.h"
#include "NameManager.h"
#include "ImGuizmo.h"
#include <set>
using json = nlohmann::json;

PrefabManager::PrefabManager(ECS::World* currentWorld, std::shared_ptr<NameManager> nameManager)
{
	m_CurrentWorld = currentWorld;
	m_NameManager = nameManager;
}

PrefabManager::~PrefabManager()
{
}

void PrefabManager::SavePrefab(ECS::Entity* _selectedEntity, const std::string& _filename)
{
	json prefabData;

	RecursiveSaveComponents(_selectedEntity, prefabData);

	std::ofstream outputFile(_filename);
	outputFile << std::setw(4) << prefabData << std::endl;

	outputFile.close();
}


ECS::Entity* PrefabManager::LoadPrefab(const std::string& _filename)
{
	std::ifstream inputFile(_filename);
	json prefabData;
	inputFile >> prefabData;
	inputFile.close();

	for (const auto& entity : prefabData["Prefabs"])
	{
		for (auto it = entity.begin(); it != entity.end(); ++it)
		{
			Entity* prefabEntity = m_CurrentWorld->create();
			int oldID = 0;
			for (const auto& component : it.value())
			{
				std::string componentName = component.begin().key();
				if (componentName == "EntityIdentifier")
				{
					prefabEntity->Assign<EntityIdentifier>(prefabEntity->getEntityId());
					oldID = component["EntityIdentifier"][0]["m_EntityId"];
					prefabEntity->get<EntityIdentifier>()->m_ComponentName = component["EntityIdentifier"][0]["m_ComponentName"];
					prefabEntity->get<EntityIdentifier>()->m_EntityName = component["EntityIdentifier"][0]["m_EntityName"];
					prefabEntity->get<EntityIdentifier>()->m_HasParent = component["EntityIdentifier"][0]["m_HasParent"];
					prefabEntity->get<EntityIdentifier>()->m_ParentEntityId = component["EntityIdentifier"][0]["m_ParentEntityId"];
				}
				else if (componentName == "Transform")
				{
					AssignComponents<Transform>(prefabEntity, component["Transform"][0]);
				}
				else if (componentName == "BoxCollider")
				{
					AssignComponents<BoxCollider>(prefabEntity, component["BoxCollider"][0]);
				}

				else if (componentName == "Camera")
				{
					AssignComponents<Camera>(prefabEntity, component["Camera"][0]);
				}

				else if (componentName == "Light")
				{ 
					LightType type = component["Light"][0]["m_Type"];
					prefabEntity->Assign<Light>(type);
					prefabEntity->get<Light>().get().m_ComponentName = component["Light"][0]["m_ComponentName"];
					prefabEntity->get<Light>().get().m_Color = component["Light"][0]["m_Color"];
					prefabEntity->get<Light>().get().m_Direction = component["Light"][0]["m_Direction"];
					prefabEntity->get<Light>().get().m_Intensity = component["Light"][0]["m_Intensity"];
					prefabEntity->get<Light>().get().m_Radius = component["Light"][0]["m_Radius"];
				}

				else if (componentName == "Movement")
				{
					AssignComponents<Movement>(prefabEntity, component["Movement"][0]);
				}

				else if (componentName == "StaticMesh")
				{
					std::string fileName = component["StaticMesh"][0]["m_FileName"];
					prefabEntity->Assign<StaticMesh>(fileName);
					prefabEntity->get<StaticMesh>().get().m_ComponentName = component["StaticMesh"][0]["m_ComponentName"];
					prefabEntity->get<StaticMesh>().get().m_FileName = component["StaticMesh"][0]["m_FileName"];
					prefabEntity->get<StaticMesh>().get().m_IsModelCreated = component["StaticMesh"][0]["m_IsModelCreated"];
				}
				else if (componentName == "Debug")
				{
					AssignComponents<Debug>(prefabEntity, component["Debug"][0]);
				}
				else if (componentName == "Sound")
				{
					AssignComponents<Sound>(prefabEntity, component["Sound"][0]);
				}
				else if (componentName == "RigidBody")
				{
					AssignComponents<RigidBody>(prefabEntity, component["RigidBody"][0]);
				}
				else if (componentName == "UI")
				{
					AssignComponents<UI>(prefabEntity, component["UI"][0]);
				}
				else if (componentName == "Space")
				{
					AssignComponents<Space>(prefabEntity, component["Space"][0]);
				}
				else if (componentName == "DynamicText")
				{
					int index = component["DynamicText"][0]["m_TextNum"];
					prefabEntity->Assign<DynamicText>(index);
					prefabEntity->get<DynamicText>().get().m_ComponentName = component["DynamicText"][0]["m_ComponentName"];
					prefabEntity->get<DynamicText>().get().m_IsTextShow = component["DynamicText"][0]["m_IsTextShow"];
					prefabEntity->get<DynamicText>().get().m_CurrentTextIndex = component["DynamicText"][0]["m_CurrentTextIndex"];
					prefabEntity->get<DynamicText>().get().m_Text = component["DynamicText"][0]["m_Text"];
					prefabEntity->get<DynamicText>().get().m_TriggerDistance = component["DynamicText"][0]["m_TriggerDistance"];

					auto dynamicTextJson = component["DynamicText"];
					if (!dynamicTextJson.is_null() && dynamicTextJson.size() > 0 && dynamicTextJson[0].contains("m_FontIndex")) {
						prefabEntity->get<DynamicText>().get().m_FontIndex = dynamicTextJson[0]["m_FontIndex"];
					}
					//prefabEntity->get<DynamicText>().get().m_FontIndex = component["DynamicText"][0]["m_FontIndex"];

					if (!dynamicTextJson.is_null() && dynamicTextJson.size() > 0 && dynamicTextJson[0].contains("m_Page")) {
						prefabEntity->get<DynamicText>().get().m_Page = dynamicTextJson[0]["m_Page"];
					}
					//prefabEntity->get<DynamicText>().get().m_Page = component["DynamicText"][0]["m_Page"];

					if (!dynamicTextJson.is_null() && dynamicTextJson.size() > 0 && dynamicTextJson[0].contains("m_Index")) {
						prefabEntity->get<DynamicText>().get().m_Index = dynamicTextJson[0]["m_Index"];
					}
					//prefabEntity->get<DynamicText>().get().m_Index = component["DynamicText"][0]["m_Index"];
				}
				else if (componentName == "Sprite2D")
				{
					AssignComponents<Sprite2D>(prefabEntity, component["Sprite2D"][0]);
				}
				else if (componentName == "PlayerInformation")
				{
					AssignComponents<PlayerInformation>(prefabEntity, component["PlayerInformation"][0]);
				}
				else if (componentName == "Interactive")
				{
					AssignComponents<Interactive>(prefabEntity, component["Interactive"][0]);
				}
				else if (componentName == "Clue")
				{
					AssignComponents<Clue>(prefabEntity, component["Clue"][0]);
				}
				else if (componentName == "Script")
				{
					if (component["Script"][0]["m_ComponentName"].get<std::string>() == "FreeCameraScript")
					{
						AssignComponents<FreeCameraScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "SampleScript")
					{
						AssignComponents<SampleScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "PlayerScript")
					{
						AssignComponents<PlayerScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "POVCameraScript")
					{
						AssignComponents<POVCameraScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "TestUIScript")
					{
						AssignComponents<TestUIScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "DynamicTextScript")
					{
						AssignComponents<DynamicTextScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "IntroCameraScript")
					{
						AssignComponents<IntroCameraScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "OutroScript")
					{
						AssignComponents<OutroScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "DrawerScript")
					{
						AssignComponents<DrawerScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "IntroDoorScript")
					{
						AssignComponents<IntroDoorScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "DoorScript")
					{
						AssignComponents<DoorScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "IntroButtonScript")
					{
						AssignComponents<IntroButtonScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "PauseScript")
					{
						AssignComponents<PauseScript>(prefabEntity, component["Script"][0]);
					}
					else if (component["Script"][0]["m_ComponentName"].get<std::string>() == "ClueSpriteScript")
					{
						AssignComponents<ClueSpriteScript>(prefabEntity, component["Script"][0]);
					}
				}
			}
			m_prefabContainer.push_back({ prefabEntity, oldID });
		}
	}


	for (const auto& prefabChild : m_prefabContainer)
	{
		for (const auto& prefabParent : m_prefabContainer)
		{
			if (prefabChild.first->get<EntityIdentifier>().get().m_HasParent == true)
			{
				if (prefabChild.first->get<EntityIdentifier>().get().m_ParentEntityId == prefabParent.second)
				{
					SetParent(prefabChild.first, prefabParent.first);
					break;
				}
			}
		}
	}

	for (const auto& prefab : m_prefabContainer)
	{
		m_NameManager->AddEntityName(prefab.first);
	}

	m_prefabContainer[0].first->get<EntityIdentifier>()->m_HasParent = false;
	m_prefabContainer[0].first->get<EntityIdentifier>()->m_ParentEntityId = 0;

	m_prefabContainer.clear();

	return nullptr;
}

void PrefabManager::SetParent(ECS::Entity* child, ECS::Entity* parent)
{
	child->get<EntityIdentifier>().get().m_ParentEntityId = parent->getEntityId();
	child->get<EntityIdentifier>().get().m_HasParent = true;

	parent->addChild(child);
}

void PrefabManager::DeleteAllDataInJsonFile(const std::string& filename)
{
	std::string fullPath = basePath + filename;

	json emptyJson;

	std::ofstream outputFile(fullPath);

	outputFile << emptyJson.dump(4);
	outputFile.close();
}

void PrefabManager::SetContext(ECS::World* world)
{
	m_CurrentWorld = world;
}

ECS::Entity* PrefabManager::FindEntityByName(std::string entityName)
{
	for (const auto& entity : m_CurrentWorld->GetEntities())
	{
		if (entity->get<EntityIdentifier>().get().m_EntityName == entityName)
			return entity;
	}	
}

void PrefabManager::RecursiveSaveComponents(ECS::Entity* entity, json& prefabData)
{
	SaveComponents<EntityIdentifier>(entity, prefabData);
	SaveComponents<Transform>(entity, prefabData);
	SaveComponents<StaticMesh>(entity, prefabData);
	SaveComponents<BoxCollider>(entity, prefabData);
	SaveComponents<Camera>(entity, prefabData);
	SaveComponents<Light>(entity, prefabData);
	SaveComponents<Movement>(entity, prefabData);
	SaveComponents<Debug>(entity, prefabData);
	SaveComponents<Sound>(entity, prefabData);
	SaveComponents<Sprite2D>(entity, prefabData);
	SaveComponents<Script>(entity, prefabData);
	SaveComponents<RigidBody>(entity, prefabData);
	SaveComponents<UI>(entity, prefabData);
	SaveComponents<Space>(entity, prefabData);
	SaveComponents<DynamicText>(entity, prefabData);
	SaveComponents<PlayerInformation>(entity, prefabData);
	SaveComponents<Interactive>(entity, prefabData);
	SaveComponents<Clue>(entity, prefabData);

	if (!entity->m_children.empty())
	{
		for (const auto& child : entity->m_children)
		{
			RecursiveSaveComponents(child, prefabData);
		}
	}
}
