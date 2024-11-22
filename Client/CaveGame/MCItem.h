#pragma once

#include "pch.h"
#include <Texture2D.h>

class MCTilemap;

class MCItem
{
protected:
	std::string m_name;
	std::string m_iconTexture;
	int m_maxStackSize;

public:
	MCItem(std::string name, std::string k_texture);
	virtual ~MCItem();

	std::string GetName() const;
	int GetMaxStacksize() const;
	std::string GetIconTexture() const;
	virtual bool OnUseItem(MCTilemap* world, const glm::vec3& target_pos);
};