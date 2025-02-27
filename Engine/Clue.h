#pragma once
#include "ECS.h"

struct Clue
{
	ECS_DECLARE_TYPE
		Clue() = default;
	Clue(const std::string& path, const int page = 0, const int index = 0)
		: m_Page(page)
		, m_Index(index)
		, m_FilePath(path)
	{}
	~Clue() = default;

	int m_Page;
	int m_Index;
	std::string m_FilePath;

	bool m_IsBookOpened = false;
	bool m_render = false;

	std::string m_ComponentName = "Clue";
	NLOHMANN_DEFINE_TYPE_INTRUSIVE(Clue, m_ComponentName, m_Page, m_Index, m_FilePath)
};
ECS_DEFINE_TYPE(Clue)