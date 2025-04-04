#pragma once
#include "ECS.h"
#include "Vector3D.h"

enum  CollisionState
{
	ENTER,
	STAY,
	EXIT,
	NONE
};

enum ColliderType
{
	DYNAMIC,
	STATIC
};

enum CollisionType
{
	PLAYER = (0 << 0),
	WALL = (1 << 0),
	GROUND = (2 << 0),
	OBJECT = (3 << 0),
	TRIGGER = (4 << 0),
	ROOM = (5 << 0)
};

struct BoxCollider
{
	ECS_DECLARE_TYPE

	//BoxCollider() = default;

	BoxCollider(ColliderType type = ColliderType::STATIC,CollisionType collisionmask = CollisionType::TRIGGER, Vector3D scale = {1.0f, 1.0f, 1.0f})
		: m_ColliderType(type)
		, m_CollisionType(collisionmask)
		, m_Size(scale)
	{}

	std::string m_ComponentName = "BoxCollider";
	ColliderType m_ColliderType;
	CollisionType m_CollisionType;
	CollisionState m_State = CollisionState::NONE;
	CollisionState m_prevState = CollisionState::NONE;
	Vector3D m_WorldPosition = { 0.f,0.f,0.f };
	Vector3D m_Center = { 0.f,0.f,0.f };
	Vector3D m_Size = { 1.f,1.f,1.f };
	Vector3D m_Rotation = { 0.f,0.f,0.f };
	bool m_WasRaycastHit = false;
	bool m_IsRaycastHit = false;

	NLOHMANN_DEFINE_TYPE_INTRUSIVE(BoxCollider, m_ComponentName, m_ColliderType, m_CollisionType, m_Center, m_Size, m_Rotation)
};

ECS_DEFINE_TYPE(BoxCollider)

