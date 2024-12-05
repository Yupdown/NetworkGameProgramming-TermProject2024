#include "pch.h"
#include "MCItemManager.h"
#include "MCItemTable.h"
#include "NetworkMgr.h"
#include "Hero.h"

MCItemManager::MCItemManager()
{
}

MCItemManager::~MCItemManager()
{
}

void MCItemManager::LoadItems()
{
	m_registry.Insert("block_dirt", make_shared<MCItemBlock>("Dirt", "tile_preview_02.png", 2));
	m_registry.Insert("block_cobblestone", make_shared<MCItemBlock>("Cobblestone", "tile_preview_04.png", 4));
	m_registry.Insert("block_wooden_planks", make_shared<MCItemBlock>("Wooden Planks", "tile_preview_05.png", 5));
	m_registry.Insert("block_bricks", make_shared<MCItemBlock>("Bricks", "tile_preview_06.png", 6));
	m_registry.Insert("block_wood", make_shared<MCItemBlock>("Wood", "tile_preview_07.png", 7));
	m_registry.Insert("block_glass", make_shared<MCItemBlock>("Glass", "tile_preview_09.png", 9));
	m_registry.Insert("eye", make_shared<MCItem>("Ender Eye", "item_eye.png"));
	m_registry.Insert("bow", make_shared<MCItemCallback>("Bow", "bow.png", [](MCTilemap*, Hero* player, const RaycastResult&) {
		player->Fire();
		}));
	m_registry.Insert("spawn_egg", make_shared<MCItemCallback>("Spawn Egg", "item_egg.png", [](MCTilemap*, Hero* player, const RaycastResult&) {
		Send(c2s_SUMMON_BOSS{});
		}));

	// 나중에 다형성을 쓰고싶다면
	// MCItemTable::CreateItemForClient<MCArrow>("arrow")

	//for (const auto& [key, val] : MCItemTable::GetStrTable())
	//{
	//	// m_registry.Insert(key, make_shared<MCItem>(val.first, val.second));

	//	m_registry.Insert(key.data(), MCItemTable::CreateItemForClient<MCItem>(key));
	//}
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
