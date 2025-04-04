#include "pch.h"
#include "SceneHierarchyPanel.h"

// Component Headers 
#include "../Engine/Transform.h"
#include "../Engine/Camera.h"
#include "../Engine/EntityIdentifier.h"
#include "../Engine/Light.h"
#include "../Engine/Script.h"
#include "../Engine/BoxCollider.h"
#include "../Engine/StaticMesh.h"
#include "../Engine/Debug.h"
#include "../Engine/Movement.h"
#include "../Engine/RigidBody.h"
#include "../Engine/Sound.h"
#include "../Engine/Sprite2D.h"
#include "../Engine/UI.h"
#include "../Engine/Space.h"
#include "../Engine/DynamicText.h"
#include "../Engine/PlayerInformation.h"
#include "../Engine/Interactive.h"
#include "../Engine/Clue.h"

#include "../Engine/PhysicsManager.h"

#include "Prefab.h"
#include "NameManager.h"
#include "ImGuizmo.h"

#include <cassert>
#include <codecvt>

#include "../Engine/RenderManager.h"

SceneHierarchyPanel::SceneHierarchyPanel(ECS::World* context)
{
	SetContext(context, m_PrefabManager, m_NameManager);
}

SceneHierarchyPanel::~SceneHierarchyPanel()
{
	std::remove("../Resource/CopiedEntity/CopiedEntity.json");
}

void SceneHierarchyPanel::SetContext(ECS::World* context, std::shared_ptr<PrefabManager> prefab, std::shared_ptr<NameManager> nameManager)
{
	m_Context = context;
	m_SelectionContext = nullptr;	// 현재 World에서 Entity 를 초기화.

	m_PrefabManager = prefab;
	m_NameManager = nameManager;
}

void SceneHierarchyPanel::RenderImGui()
{
	ImGui::Begin("Scene Hierarchy");

	if (m_Context)
	{
		//현재 씬이 가지고 있는 모든 Entity 의 정보를 그린다.
		for (auto entity : m_Context->GetEntities())
		{
			// 최상위 부모로 등록된 애들만 먼저 그림
			if (entity->m_parent == nullptr)
				DrawEntityNode(entity);
		}

		// Unselect object when left-clicking on blank space.
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = nullptr;

		// Right-click on blank space
		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				ECS::Entity* entity = m_Context->create();

				entity->Assign<EntityIdentifier>(entity->getEntityId());	// 기본적으로 생성한다. (이름정보 때문)
				m_NameManager->AddEntityName(entity);
				entity->Assign<Transform>();	// 에디터에서 오브젝트의 위치를 조정하기위해 Transform은 기본적으로 생성해준다.
				//entity->Assign<StaticMesh>();	// 아무 파일 경로도 가지지 않은 빈 StaticMesh 기본적으로 생성
			}

			ImGui::EndPopup();
		}
	}

	ImGuiIO& io = ImGui::GetIO();
	if (m_SelectionContext)
	{
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C))
		{
			//m_SelectionContext->get<EntityIdentifier>()->m_HasParent = false;
			//m_SelectionContext->get<EntityIdentifier>()->m_ParentEntityId = 0;
			m_PrefabManager->SavePrefab(m_SelectionContext, "../Resource/CopiedEntity/CopiedEntity.json");
		}
	}

	bool isFileExists = FileExists("../Resource/CopiedEntity/CopiedEntity.json");
	if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V) && isFileExists)
	{
		m_PrefabManager->LoadPrefab("../Resource/CopiedEntity/CopiedEntity.json");
	}

	ImGui::End();	/* Hierarchy End */

	ImGui::Begin("Properties");		
	// 선택된 오브젝트가 가진 모든 컴포넌트 정보를 출력한다.
	if (m_SelectionContext)
	{
		DrawComponents(m_SelectionContext);
		SetPrefabFileName(m_SelectionContext);
	}
	ImGui::End();	/* Properties End */
}

void SceneHierarchyPanel::SetPrefabFileName(ECS::Entity* entity)
{
	if (m_OpenTextPopup)
	{
		ImGui::SetNextWindowSize(ImVec2(320, 120));
		ImGui::OpenPopup("Prefab Name");
		if (ImGui::BeginPopupModal("Prefab Name"))
		{
			static char prefabName[256] = ""; // Fixed-size buffer for input

			ImGui::InputText("Prefab Name", prefabName, sizeof(prefabName));
			//ImGui::EndGroup();
			ImGui::Spacing();
			//ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Submit").x - 250.f);
			if (ImGui::Button("Submit"))
			{
				std::string prefabFile = prefabName;
				prefabFile += ".prefab";
				m_PrefabManager.get()->SavePrefab(entity, "../Resource/prefab/" + prefabFile);
				ImGui::CloseCurrentPopup();
				m_SelectionContext = nullptr;
				m_OpenTextPopup = false;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				std::string prefabFile = prefabName;
				prefabFile += ".prefab";
				m_PrefabManager.get()->SavePrefab(entity, "../Resource/prefab/" + prefabFile);
				ImGui::CloseCurrentPopup();
				m_SelectionContext = nullptr;
				m_OpenTextPopup = false;
			}

			ImGui::Spacing();

			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup(); 
				m_SelectionContext = nullptr;
				m_OpenTextPopup = false;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				ImGui::CloseCurrentPopup();
				m_SelectionContext = nullptr;
				m_OpenTextPopup = false;
			}



			ImGui::EndPopup();
		}
	}
}

void SceneHierarchyPanel::DragDropEntityHierarchy(ECS::Entity* entity)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		//const auto& selectedEntityID = m_SelectionContext->getEntityId();
		size_t entityID = entity->getEntityId();

		//if (m_SelectionContext != entity)
		//{
			ImGui::Text(entity->get<EntityIdentifier>()->m_EntityName.c_str());
			ImGui::SetDragDropPayload("EntityID", &entityID, 1 * sizeof(size_t));
		//}
		//else
		//{
		//	ECS::Entity* entt = m_Context->getByIndex(selectedEntityID);
		//	const char* name = entt->get<EntityIdentifier>()->m_EntityName.c_str();
		//	ImGui::Text(name);

		//	ImGui::SetDragDropPayload("EntityID", &selectedEntityID, 1 * sizeof(size_t));
		//}

		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("EntityID", ImGuiDragDropFlags_AcceptNoDrawDefaultRect);

		if (payLoad)
		{
			size_t pickedID = *(static_cast<size_t*>(payLoad->Data));

			ECS::Entity* picked = m_Context->getById(pickedID);
			ECS::Entity* target = entity;

			// 자기 자식의 자식으로 등록하려는 경우 아무런 처리를 하지 않는다. (이거 허용하면 엔티티가 삭제됨.)
			if (picked->isDescendant(target))
			{
				return;
			}

			//picked->get<EntityIdentifier>().get().m_ParentEntityId = target->getEntityId();
			//picked->get<EntityIdentifier>().get().m_HasParent = true;
			//target->addChild(picked);

			SetParent(picked, target);

			m_SelectionContext = nullptr;
			
		}

		ImGui::EndDragDropTarget();
	}
}

bool SceneHierarchyPanel::FileExists(const std::string& filename)
{
	std::ifstream file(filename);
	return file.good();
}

void SceneHierarchyPanel::DrawEntityNode(ECS::Entity* entity)			// 포인터로 받지 않으면 함수 종료시 객체의 소멸자가 호출되어서 오류가 뜰 수 있음.
{
	bool temp = entity->has<EntityIdentifier>();
	if (!temp)
	{
		return;
	}
	std::string entID = std::to_string(entity->getEntityId());
	auto entityName = entity->get<EntityIdentifier>()->m_EntityName;
	std::string imguiID = entityName;

	ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;		// 클릭하여 선택한 아이템 하이라이트 + 화살표 클릭시 노드 펼쳐지게
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, imguiID.c_str());
	if (ImGui::IsItemClicked())
	{
		m_SelectionContext = entity;
	}

	// Entity 삭제 메뉴
	bool entityDeleted = false;
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Delete Entity"))
		{
			entityDeleted = true;
		}

		if (ImGui::MenuItem("Make Prefab"))
		{
			m_OpenTextPopup = true;
			m_SelectionContext = entity;
		}

		if (ImGui::MenuItem("Set Top Level Parent"))
		{
			if (entity->m_parent != nullptr)
			{
				ResetTransform(entity, entity->m_parent);



			
			}
		}

		ImGui::EndPopup();
	}

	DragDropEntityHierarchy(entity);


	// 노드가 펼쳐졌다면 자식도 출력.
	if (opened)
	{
		for (const auto& child : entity->m_children)
		{
			DrawEntityNode(child);
		}

		ImGui::TreePop();
	}

	// Entity 삭제
	if (entityDeleted)
	{
		m_NameManager->DeleteEntityName(entity);
		m_Context->destroy(entity);
		if (m_SelectionContext == entity)
			m_SelectionContext = nullptr;
	}

}

static void DrawVec3Control(const std::string& label, Vector3D& values, float resetValue = 0.0f, float columnWidth = 100.0f)
{
	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[0];

	ImGui::PushID(label.c_str());

	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
	ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("X", buttonSize))
		values.SetX(resetValue); //
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();

	float tempX = values.GetX();//

	ImGui::DragFloat("##X", &tempX, 0.1f, 0.0f, 0.0f, "%.2f"); //
	values.SetX(tempX);	//

	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Y", buttonSize))
		values.SetY(resetValue);
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	float tempY = values.GetY();	//
	ImGui::DragFloat("##Y", &tempY, 0.1f, 0.0f, 0.0f, "%.2f");	//
	values.SetY(tempY);	//
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);
	if (ImGui::Button("Z", buttonSize))
		values.SetZ(resetValue);
	ImGui::PopFont();
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	float tempZ = values.GetZ();	//
	ImGui::DragFloat("##Z", &tempZ, 0.1f, 0.0f, 0.0f, "%.2f");	//
	values.SetZ(tempZ);	//
	ImGui::PopItemWidth();

	ImGui::PopStyleVar();

	ImGui::Columns(1);

	ImGui::PopID();
}

template <typename T, typename UIFunction>
static void DrawComponent(const std::string& name, ECS::Entity* entity, UIFunction uiFunction)
{
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
	if (entity->has<T>())
	{
		auto component = entity->get<T>();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImGui::Separator();
		bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
		ImGui::PopStyleVar(
		);
		ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
		if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
		{
			ImGui::OpenPopup("ComponentSettings");
		}

		bool removeComponent = false;
		if (ImGui::BeginPopup("ComponentSettings"))
		{
			if (ImGui::MenuItem("Remove component"))
				removeComponent = true;

			ImGui::EndPopup();
		}

		if (open)
		{
			uiFunction(component.component);
			ImGui::TreePop();
		}

		if (removeComponent)
		{
			if(typeid(T) != typeid(Transform) && typeid(T) != typeid(EntityIdentifier))
				entity->remove<T>();
		}
	}
	
}

void SceneHierarchyPanel::DrawComponents(ECS::Entity* entity)
{
	// Type Entity's Name
	if (entity->has<EntityIdentifier>())
	{
		auto& testentityID = entity->get<EntityIdentifier>()->m_EntityName;
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strncpy_s(buffer, sizeof(buffer), testentityID.c_str(), sizeof(buffer));
		if (ImGui::InputText("##Name", buffer, sizeof(buffer)))
		{
			testentityID = buffer;
		}
	}

	ImGui::PushItemWidth(-1);
	ImGui::SameLine();

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent"))
	{
		// 추가할 수 있는 모든 컴포넌트 목록을 출력하고 선택하면 추가한다.
		DisplayAddComponentEntry<Transform>("Transform");
		DisplayAddComponentEntry<StaticMesh>("StaticMesh");
		DisplayAddComponentEntry<BoxCollider>("BoxCollider");
		DisplayAddComponentEntry<Camera>("Camera");
		DisplayAddComponentEntry<Light>("Light");
		DisplayAddComponentEntry<Script>("Script");
		DisplayAddComponentEntry<Movement>("Movement");
		DisplayAddComponentEntry<RigidBody>("RigidBody");
		DisplayAddComponentEntry<Space>("Space");
		DisplayAddComponentEntry<Sprite2D>("Sprite2D");
		DisplayAddComponentEntry<Debug>("Debug");
		DisplayAddComponentEntry<UI>("UI");
		DisplayAddComponentEntry<DynamicText>("DynamicText");
		DisplayAddComponentEntry<Sound>("Sound");
		DisplayAddComponentEntry<PlayerInformation>("PlayerInformation");
		DisplayAddComponentEntry<Interactive>("Interactive");
		DisplayAddComponentEntry <Clue>("Clue");
		ImGui::EndPopup();
	}

	if (m_AssignStaticMesh)
		ShowDialog<StaticMesh>();
	else if (m_AssignSprite2D)
		ShowDialog<Sprite2D>();
	else if (m_ShowPngDialong)
		PngDialong();
	else if (m_AssignClue)
		ShowDialog<Clue>();

	ImGui::PopItemWidth();

	DrawComponent<EntityIdentifier>("EntityIdentifier", entity, [](auto component)
	{
			std::string entityName = "EntityName : " + component->m_EntityName;
			ImGui::Text(entityName.c_str());
			std::string entityID = "EntityID : " + std::to_string(component->m_EntityId);
			ImGui::Text(entityID.c_str());
			std::string trueOrFalse = component->m_HasParent ? "true" : "false";
			std::string HasParent = "HasParent : " + trueOrFalse;
			ImGui::Text(HasParent.c_str());
			std::string parentID = "ParentID : " + std::to_string(component->m_ParentEntityId);
			ImGui::Text(parentID.c_str());
	});

	DrawComponent<Transform>("Transform", entity, [](auto component)
	{
		DrawVec3Control("Position", component->m_Position);
		DrawVec3Control("Rotation", component->m_Rotation);
		DrawVec3Control("Scale", component->m_Scale, 1.0f);
	});

	DrawComponent<StaticMesh>("StaticMesh", entity, [](auto component)
	{
		std::string temp = component->m_FileName;
		ImGui::Text(temp.c_str());
	});

	DrawComponent<BoxCollider>("BoxCollider", entity, [entity](auto component)
	{
		// Collider Type Combo Box
		const char* ColliderTypeStrings[] = { "Dynamic", "Static", "Plane" };
		const char* currentColliderTypeString = ColliderTypeStrings[(int)component->m_ColliderType];
		ImGui::SetNextItemWidth(150.f);

		if (ImGui::BeginCombo("Collider Type", currentColliderTypeString))
		{
			for (int i = 0; i < 3; i++)
			{
				bool isSelected = currentColliderTypeString == ColliderTypeStrings[i];
				if (ImGui::Selectable(ColliderTypeStrings[i], isSelected))
				{
					currentColliderTypeString = ColliderTypeStrings[i];
					component->m_ColliderType = static_cast<ColliderType>(i);

					PhysicsManager::GetInstance()->ChangeCollider(component, entity->getEntityId());
#ifdef _DEBUG
					cout << "Collider Type Changed" << endl;
#endif
				}

				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		// CollisionType Combo Box
		const char* CollisionTypeStrings[] = { "Player", "Wall", "Ground",  "Slope" , "Object", "Trigger" };
		const char* currentCollisionTypeString = CollisionTypeStrings[(int)component->m_CollisionType];
		ImGui::SetNextItemWidth(150.f);

		if (ImGui::BeginCombo("Collision Type Type", currentCollisionTypeString))
		{
			for (int i = 0; i < 6; i++)
			{
				bool isSelected = currentCollisionTypeString == CollisionTypeStrings[i];
				if (ImGui::Selectable(CollisionTypeStrings[i], isSelected))
				{
					currentCollisionTypeString = CollisionTypeStrings[i];
					component->m_CollisionType = static_cast<CollisionType>(i);

					PhysicsManager::GetInstance()->ChangeFilter(entity->getEntityId());
#ifdef _DEBUG
					cout << "Collision Type Changed" << endl;
#endif
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		DrawVec3Control("Center", component->m_Center);
		DrawVec3Control("Size", component->m_Size);
		DrawVec3Control("Rotation", component->m_Rotation);

	});

	DrawComponent<Camera>("Camera", entity, [](auto component)
	{

	});

	DrawComponent<Light>("Light", entity, [&](auto component)
	{
		const char* lightTypeStrings[] = { "Directional Light", "Point Light" };
		const char* currentLightTypeString = lightTypeStrings[(int)component->m_Type];
		ImGui::SetNextItemWidth(150.f);

		if (ImGui::BeginCombo("Light Type", currentLightTypeString))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentLightTypeString == lightTypeStrings[i];
				if (ImGui::Selectable(lightTypeStrings[i], isSelected))
				{
					currentLightTypeString = lightTypeStrings[i];
					component->m_Type = static_cast<LightType>(i);
					RenderManager::GetInstance()->ChangeLight(entity->getEntityId(), component);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if(component->m_Type == LightType::Directional)
		{
			DrawVec3Control("DL_Color", component->m_Color);
			//DrawVec3Control("DL_Direction", component->m_Direction);
			float dir[3] = { component->m_Direction.m_X, component->m_Direction.m_Y, component->m_Direction.m_Z };
			ImGui::DragFloat3("DL_Direction", dir, 1, -1, 1);
			component->m_Direction.m_X = dir[0];
			component->m_Direction.m_Y = dir[1];
			component->m_Direction.m_Z = dir[2];
			ImGui::AlignTextToFramePadding();
		}

		if (component->m_Type == LightType::Point)
		{
			DrawVec3Control("PL_Color", component->m_Color);
			ImGui::DragFloat("PL_Intensity", &component->m_Intensity, 1, 0.f);
			ImGui::DragFloat("PL_Radius", &component->m_Radius, 1, 0.f);
			ImGui::AlignTextToFramePadding();
		}
	});

	DrawComponent<Script>("Script", entity, [](auto component)
	{
		const char* scripts[] = { 
			"FreeCameraScript"
			, "SampleScript"
			, "PlayerScript"
			, "POVCameraScript"
			, "TestUIScript"
			, "DynamicTextScript"
			, "IntroCameraScript"
			, "OutroScript"
			, "DrawerScript"
			, "IntroDoorScript"
			, "DoorScript"
			, "IntroButtonScript"
			, "PauseScript"
			, "ClueSpriteScript"};
		//요기
		static int item_current = 1;
		ImGui::ListBox("ScriptList", &item_current, scripts, IM_ARRAYSIZE(scripts), 4);

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (item_current >= 0 && item_current < IM_ARRAYSIZE(scripts)) // Check if the index is valid
			{
				component->m_ComponentName = scripts[item_current]; // Assign the selected script name
			}
		}
		std::string s = "SelectedScript : " + component->m_ComponentName;

		ImGui::Text(s.c_str());
	});

	DrawComponent<Debug>("Debug", entity, [](auto component)
	{
		ImGui::Text("For Debug");
	});

	DrawComponent<Movement>("MoveMent", entity, [](auto component)
	{
		ImGui::SliderFloat("Speed", &component->m_Speed, 0.0f, 1000.0f);
		ImGui::SliderFloat("Sensitivity", &component->m_Sensitivity, 0.0f, 0.1f);

	});

	DrawComponent<RigidBody>("RigidBody", entity, [](auto component)
	{
		//float vellocity = component->m_MaxVellocity;
		//char array[10];
		//sprintf_s(array, "%f", vellocity);

		//ImGui::Text("Max Vellocity :");
		//ImGui::SameLine();
		//ImGui::Text(array);
	});

	DrawComponent<DynamicText>("DynamicText", entity, [](auto component)
	{
		ImGui::InputFloat("Trigger Distance:", &component->m_TriggerDistance, 0.f, 1000.f);
		ImGui::InputInt("TextIndex", &component->m_TextNum);
		ImGui::InputInt("FontIndex", &component->m_FontIndex);
		ImGui::InputInt("Page", &component->m_Page);
		ImGui::InputInt("Index", &component->m_Index);
	});

	DrawComponent<Space>("Space", entity, [](auto component)
	{
		std::string trueOrFalse = component->m_IsPlayerExist ? "true" : "false";
		std::string PlayerExists = "PlayerExist : " + trueOrFalse;
		ImGui::Text(PlayerExists.c_str());

		ImGui::SetNextItemWidth(75.f);
		ImGui::InputInt("Space Index", &component->m_SpaceIndex);
		std::string spaceIdx = "Space Index : " + std::to_string(component->m_SpaceIndex);
		ImGui::Text(spaceIdx.c_str());

		static int exitDirectionInput = 0;
		static Vector3D distanceInput;
		ImGui::SetNextItemWidth(75.f);
		ImGui::InputInt("Exit Direction : ", &exitDirectionInput);

		DrawVec3Control("Distance", distanceInput);

		if (ImGui::Button("make ExitInfo"))
		{
			component->m_Exits.push_back(ExitInfo{ exitDirectionInput, Vector3D{distanceInput.m_X, distanceInput.m_Y, distanceInput.m_Z} });
		}

		for (size_t i = 0; i < component->m_Exits.size(); i++)
		{
			ImGui::Text("Exit %d", i);
			ImGui::Text("Direction : %d", component->m_Exits[i].m_ExitDirection);
			ImGui::Text("Distance : x: %.2f y: %.2f z: %.2f", component->m_Exits[i].m_Distance.m_X, component->m_Exits[i].m_Distance.m_Y, component->m_Exits[i].m_Distance.m_Z);
		}

	});

	DrawComponent<UI>("UI", entity, [](auto component)
	{
			int x = component->m_Size[0];
			int y = component->m_Size[1];

			ImGui::Text("Interactive Size");
			//ImGui::DragFloat("X", &x, 0.1f, 0.0f, 0.0f, "%.2f");
			//ImGui::DragFloat("Y", &y, 0.1f, 0.0f, 0.0f, "%.2f");
			ImGui::SetNextItemWidth(100.f);
			ImGui::DragInt("X", &x, 0.1f, 0, 0, "%d");
			ImGui::SetNextItemWidth(100.f);
			ImGui::DragInt("Y", &y, 0.1f, 0, 0, "%d");

			component->m_Size[0] = x;
			component->m_Size[1] = y;

			const char* uiTypeStrings[] = { "HOVER", "CLICK", "HOVER", "NONE" };
			const char* currentUiTypeString = uiTypeStrings[(int)component->m_UIstate];

			ImGui::Text("State :");
			ImGui::SameLine();
			ImGui::Text(currentUiTypeString);
	});

	DrawComponent<Sprite2D>("Sprite2D", entity, [](auto component)
	{
		std::string temp = component->m_FileName;
		ImGui::Text(temp.c_str());
		ImGui::InputText("input FileName", &component->m_FileName);
		ImGui::DragFloat("Layer :", &component->m_Layer, 0.1f, 0.f, 1.f, "%.1f");

		int posX = component->m_Position[0];
		int posY = component->m_Position[1];
		ImGui::SetNextItemWidth(100.f);
		ImGui::DragInt("X", &posX, 0.1f, 0, 0, "%d");
		ImGui::SetNextItemWidth(100.f);
		ImGui::DragInt("Y", &posY, 0.1f, 0, 0, "%d");
		component->m_Position[0] = posX;
		component->m_Position[1] = posY;

		const char* IsRendered[] = { "false", "true" };
		const char* currentIsRendered = IsRendered[(int)component->m_IsRendered];
		ImGui::SetNextItemWidth(150.f);

		if (ImGui::BeginCombo("IsRendered", currentIsRendered))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = currentIsRendered == IsRendered[i];
				if (ImGui::Selectable(IsRendered[i], isSelected))
				{
					currentIsRendered = IsRendered[i];

					bool temp;
					if (currentIsRendered == "true")
					{
						temp = true;
					}
					else
					{
						temp = false;
					}

					component->m_IsRendered = temp;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
	});

	DrawComponent<Sound>("Sound", entity, [](auto component)
	{
		std::string temp = component->m_FileName;
		ImGui::Text(temp.c_str());
		ImGui::DragFloat("Volume", &component->m_Volume, 0.1f, 0.f, 0.f, "%.2f");
	});

	DrawComponent<PlayerInformation>("PlayerInformation", entity, [](auto component)
	{

	});

	DrawComponent<Clue>("Clue", entity, [](auto component)
		{
			ImGui::InputInt("Page", &component->m_Page);
			ImGui::InputInt("Index", &component->m_Index);
		});

	DrawComponent<Interactive>("Interactive", entity, [](auto component)
	{
		ImGui::InputInt("OpeningDir", &component->m_OpeningDir);
		ImGui::InputInt("DoorIndex", &component->m_DoorIndex);
	});
}

void SceneHierarchyPanel::ShowStaticModelDialog()
{
	std::string fileName;
	std::string filePathName;
	std::string filePath;

	if (m_IsDialogOpen)
	{
		IGFD::FileDialogConfig config; config.path = "../Resource/fbx";
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".fbx", config);
	}

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action

			m_SelectionContext->Assign<StaticMesh>("fbx/" + fileName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
		m_IsDialogOpen = false;
	}
}

void SceneHierarchyPanel::PngDialong()
{
	std::string fileName;
	std::string filePathName;
	std::string filePath;

	if (m_IsDialogOpen)
	{
		IGFD::FileDialogConfig config; config.path = "../Resource/UI";
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".png", config);
	}

	// display
	if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();
			filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
			filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action

			m_SelectionContext->Assign<Sprite2D>("../Resource/UI/" + fileName);
		}

		// close
		ImGuiFileDialog::Instance()->Close();
		m_IsDialogOpen = false;
	}
}

void SceneHierarchyPanel::SetParent(ECS::Entity* child, ECS::Entity* parent)
{
	child->get<EntityIdentifier>().get().m_ParentEntityId = parent->getEntityId();
	child->get<EntityIdentifier>().get().m_HasParent = true;


	auto matrix = child->get<Transform>().get().m_RelativeMatrix.ConvertToMatrix() * DirectX::XMMatrixInverse(nullptr, parent->get<Transform>()->m_WorldMatrix.ConvertToMatrix());

	float fTranslation[3] = { 0.0f, 0.0f, 0.0f };
	float fRotation[3] = { 0.0f, 0.0f, 0.0f };
	float fScale[3] = { 0.0f, 0.0f, 0.0f };

	ImGuizmo::DecomposeMatrixToComponents(*matrix.m, fTranslation, fRotation, fScale);

	child->get<Transform>()->m_Position = { fTranslation[0],fTranslation[1],fTranslation[2] };
	child->get<Transform>()->m_Rotation = { fRotation[0],fRotation[1],fRotation[2] };
	child->get<Transform>()->m_Scale = { fScale[0],fScale[1],fScale[2] };

	parent->addChild(child);
}

void SceneHierarchyPanel::ResetTransform(ECS::Entity* child, ECS::Entity* parent)
{
	parent->RemoveChild(child);
	child->get<EntityIdentifier>()->m_HasParent = false;
	child->get<EntityIdentifier>()->m_ParentEntityId = 0;

	float fTranslation[3] = { 0.0f, 0.0f, 0.0f };
	float fRotation[3] = { 0.0f, 0.0f, 0.0f };
	float fScale[3] = { 0.0f, 0.0f, 0.0f };

	auto matrix = child->get<Transform>()->m_RelativeMatrix.ConvertToMatrix() * parent->get<Transform>()->m_WorldMatrix.ConvertToMatrix();

	ImGuizmo::DecomposeMatrixToComponents(*matrix.m, fTranslation, fRotation, fScale);

	child->get<Transform>()->m_Position = { fTranslation[0],fTranslation[1],fTranslation[2] };
	child->get<Transform>()->m_Rotation = { fRotation[0],fRotation[1],fRotation[2] };
	child->get<Transform>()->m_Scale = { fScale[0],fScale[1],fScale[2] };
}

