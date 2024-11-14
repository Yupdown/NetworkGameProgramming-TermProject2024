#pragma once

#include "pch.h"
#include <Scene.h>

class ServerObject;
class MCTilemap;
class Hero;

class ServerObjectManager : public Singleton<ServerObjectManager>
{
public:
	ServerObjectManager();

	void AddObject(std::shared_ptr<ServerObject> obj, GROUP_TYPE eType);
	void RemoveObject(unsigned int id);
	std::shared_ptr<ServerObject> FindObject(unsigned int id) const;

	void SetTargetScene(const std::shared_ptr<Scene>& scene);
	void SetTileMap(std::shared_ptr<MCTilemap> tile_map) { m_tileMap = std::move(tile_map); }
	const auto& GetTileMap()const noexcept { return m_tileMap; }
public:
	void SetMyID(const int id_) {
		NAGOX_ASSERT(-1 == m_my_id_from_server, "ID Must Init Once");
		m_my_id_from_server = id_;
	}
	const auto GetMyID()const noexcept { return m_my_id_from_server; }
	void SetHero(shared_ptr<Hero> hero);
	auto& GetHero()noexcept { return m_hero; }
public:
	// Like Factory
	std::shared_ptr<Player> CreatePlayer(const int id_);
private:
	std::unordered_map<unsigned int, std::shared_ptr<ServerObject>> m_serverObjects;
	std::shared_ptr<Scene> m_targetScene;

	std::shared_ptr<MCTilemap> m_tileMap;
	std::shared_ptr<Hero> m_hero;

	int m_my_id_from_server = -1;
};