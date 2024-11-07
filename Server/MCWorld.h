#pragma once
#include "pch.h"
#include "Timer.h"
#include "Singleton.hpp"

// class MCTilemap;
// class MCTerrainGenerator;
// class Object;
// class ClientSession;
// 
// // TODO: 좌표와 클리어 범위
// static constexpr const glm::vec3 G_CLEAR_POINT = { 256 ,22 ,286 };
// static constexpr const float G_CLEAR_DIST = 4.f;
// class MCWorld
// 	: public Singleton<MCWorld>
// {
// 	friend class Singleton;
// 	MCWorld();
// 	~MCWorld();
// public:
// 	const auto& GetTileMap()const noexcept { return m_tileMap; }
// 	void Init()noexcept;
// 	Protocol::MCTilemap* GetMCWorld()const noexcept;
// 	const ServerCore::S_ptr<ServerCore::SessionManageable>& GetSessionMgr()const noexcept { return m_pSessionMgr; }
// 	void Update();
// 	const auto& AddObject(S_ptr<Object> obj) {
// 		std::scoped_lock lk{ m_srwLock };
// 		return m_vecObj.emplace_back(std::move(obj));
// 	}
// public:
// 	std::atomic_int m_clientCount = 0;
// 	std::vector<S_ptr<Object>> m_vecObj;
// 	std::shared_mutex m_srwLock;
// 	std::atomic<std::shared_ptr<ClientSession>> m_arrSessions[3];
// 	volatile bool clear_flag = false;
// private:
// 	//const S_ptr<SessionManageable> m_pSessionMgr;
// 	const S_ptr<MCTilemap> m_tileMap;
// 	const S_ptr<MCTerrainGenerator> m_terrainGenerator;
// 	Timer m_timer;
// };

//#define SessionMgr(type) (SINGLE(type)->GetSessionMgr())

