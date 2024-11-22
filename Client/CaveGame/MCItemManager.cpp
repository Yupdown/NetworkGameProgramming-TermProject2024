#include "pch.h"
#include "MCItemManager.h"

MCItemManager::MCItemManager()
{
}

MCItemManager::~MCItemManager()
{
}

void MCItemManager::LoadItems()
{
	m_registry.Insert("block_stone", make_shared<MCItem>("Stone", "tile_preview_01.png"));
	m_registry.Insert("block_dirt", make_shared<MCItem>("Dirt", "tile_preview_02.png"));
	m_registry.Insert("block_grass", make_shared<MCItem>("Grass", "tile_preview_03.png"));
	m_registry.Insert("block_cobblestone", make_shared<MCItem>("Cobblestone", "tile_preview_04.png"));
	m_registry.Insert("block_wooden_planks", make_shared<MCItem>("Wooden Planks", "tile_preview_05.png"));
	m_registry.Insert("block_bricks", make_shared<MCItem>("Bricks", "tile_preview_06.png"));
	m_registry.Insert("block_wood", make_shared<MCItem>("Wood", "tile_preview_07.png"));
	m_registry.Insert("block_leaves", make_shared<MCItem>("Leaves", "tile_preview_08.png"));
	m_registry.Insert("block_glass", make_shared<MCItem>("Glass", "tile_preview_09.png"));
	m_registry.Insert("eye", make_shared<MCItem>("Ender Eye", "item_eye.png"));
	m_registry.Insert("arrow", make_shared<MCItem>("Arrow", "item_arrow.png"));
}

void MCItemManager::Initialize()
{

}

MCItem* MCItemManager::GetItemByID(int id) const
{
	return m_registry[id];
}

MCItem* MCItemManager::GetItemByKey(const std::string& key) const
{
	return m_registry[key];
}
