#pragma once
#include "ECS.h"

enum UISTATE
{
	HOVER,
	CLICK,
	HOLD,
	UINONE,
};
struct UI
{
	ECS_DECLARE_TYPE

	UI() = default;
	UI(int width, int height)
		: m_Size{width, height}
	{}
	~UI() = default;

	long m_Size[2] = { 0,0 };
	UISTATE m_UIstate = UISTATE::UINONE;
	std::string m_ComponentName = "UI";
	bool m_IsShow;

	bool m_GoBackToLobby = false;
	bool m_EndGame = false;


	NLOHMANN_DEFINE_TYPE_INTRUSIVE(UI, m_ComponentName, m_Size)
};
ECS_DEFINE_TYPE(UI);