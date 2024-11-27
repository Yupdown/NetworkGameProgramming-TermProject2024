#pragma once

#include "pch.h"
#include <Scene.h>

class ServerObject;
class MCTilemap;
class Hero;

static constexpr glm::vec3 G_INIT_POS = glm::vec3(256.0f, 16.0f, 256.0f);

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
	
	const auto& GetTargetScene()const noexcept { return m_targetScene; }
public:
	void SetMyID(const int id_) {
		NAGOX_ASSERT(-1 == m_my_id_from_server, "ID Must Init Once");
		m_my_id_from_server = id_;
	}
	const auto GetMyID()const noexcept { return m_my_id_from_server; }
	void SetHero(shared_ptr<Hero> hero);
	auto& GetHero()noexcept { return m_hero; }
	bool IsMyID(const uint32_t id_)const noexcept { return id_ == m_my_id_from_server; }
	void SetMyTexID(const int id_) { m_my_tex_id = id_; }
	const auto GetMyTexIDFromServer()const noexcept { return m_my_tex_id; }
private:
	std::unordered_map<unsigned int, std::shared_ptr<ServerObject>> m_serverObjects;

	std::shared_ptr<Scene> m_targetScene;

	std::shared_ptr<MCTilemap> m_tileMap;
	std::shared_ptr<Hero> m_hero;

	int m_my_id_from_server = -1;
	int m_my_tex_id = -1;
};

static inline const auto& GetTileMapGlobal()noexcept { return ServerObjectManager::GetInst()->GetTileMap(); }