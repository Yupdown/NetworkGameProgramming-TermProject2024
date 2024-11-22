#include "pch.h"
#include "MCItem.h"

MCItem::MCItem(std::string name, std::string k_element)
{
	m_name = name;
	m_iconTexture = k_element;
	m_maxStackSize = 99;
}

MCItem::~MCItem()
{
}

std::string MCItem::GetName() const
{
	return m_name;
}

int MCItem::GetMaxStacksize() const
{
	return m_maxStackSize;
}

std::string MCItem::GetIconTexture() const
{
	return m_iconTexture;
}

bool MCItem::OnUseItem(MCTilemap* world, const glm::vec3& target_pos)
{
	return false;
}