#pragma once

#include "pch.h"
#include <Texture2D.h>

#include "MCTilemap.h"
#include "Hero.h"

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
	virtual bool OnUseItem(MCTilemap* world, Hero* owner, const RaycastResult& raycast);
};

class MCItemBlock : public MCItem
{
public:
	MCItemBlock(std::string name, std::string k_texture, uint8_t tileID);
	bool OnUseItem(MCTilemap* world, Hero* owner, const RaycastResult& raycast) override;
private:
	uint8 m_tileID;
};

class MCItemCallback : public MCItem
{
public:
	MCItemCallback(std::string name, std::string k_texture, std::function<void(MCTilemap*, Hero*, const RaycastResult&)> callback);
	bool OnUseItem(MCTilemap* world, Hero* owner, const RaycastResult& raycast) override;
private:
	std::function<void(MCTilemap*, Hero*, const RaycastResult&)> m_callback;
};