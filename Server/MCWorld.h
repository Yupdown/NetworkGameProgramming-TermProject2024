#pragma once
#include "pch.h"
#include "Timer.h"
#include "Singleton.hpp"
#include "LFQueue.hpp"

 class MCTilemap;
 class MCTerrainGenerator;
 class Object;
 class ClientSession;
 
 // TODO: 좌표와 클리어 범위
 static constexpr const glm::vec3 G_CLEAR_POINT = { 256 ,22 ,286 };
 static constexpr const float G_CLEAR_DIST = 4.f;

 class MCWorld
 	: public Singleton<MCWorld>
 {
 	friend class Singleton;
 	MCWorld();
 	~MCWorld();
 public:
     static constexpr const int G_MC_SEED = 1;
 public:
 	const auto& GetTileMap()const noexcept { return m_tileMap; }
 	void Init()noexcept;
 	
 	void Update();
 	const auto& AddObject(S_ptr<Object> obj, MC_OBJECT_TYPE eType) {
        return m_worldObjects[static_cast<int>(eType)].emplace_back(std::move(obj));
 	}

    template<typename... Args>
    void PostWorldEvent(Args&&... args)noexcept { m_worldEventQueue.Push(std::forward<Args>(args)...); }
 
 private:
 	const S_ptr<MCTilemap> m_tileMap;
 	const S_ptr<MCTerrainGenerator> m_terrainGenerator;
    std::vector<S_ptr<Object>> m_worldObjects[static_cast<int>(MC_OBJECT_TYPE::END)];

 	Timer m_timer;

    LFQueue<std::function<void(void)>> m_worldEventQueue;
 };

