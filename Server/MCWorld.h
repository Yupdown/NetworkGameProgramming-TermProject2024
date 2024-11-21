#pragma once
#include "pch.h"
#include "Timer.h"
#include "Singleton.hpp"
#include "LFQueue.hpp"
#include "SendBuffer.h"
#include "SendBufferPool.h"

 class MCTilemap;
 class MCTerrainGenerator;
 class Object;
 class ClientSession;
 class Session;

 // TODO: 좌표와 클리어 범위
 static constexpr const glm::vec3 G_CLEAR_POINT = { 256 ,22 ,286 };
 static constexpr const float G_CLEAR_DIST = 4.f;
 static constexpr const int G_NUM_OF_MONSTERS = 100;

 class MCWorld
 	: public Singleton<MCWorld>
 {
 	friend class Singleton;
 	MCWorld();
 	~MCWorld();
 public:
     static constexpr const int G_MC_SEED = 1;
     static constexpr const float UPDATE_INTERVAL = 0.1f;
 public:


 public:
 	const auto& GetTileMap()const noexcept { return m_tileMap; }
 	void Init()noexcept;
 	
    auto& GetTileMap()noexcept { return m_tileMap; }

 	void Update();

    const S_ptr<Object>& AddObject(S_ptr<Object> obj, const MC_OBJECT_TYPE eType)noexcept;

    template<typename... Args>
    void PostWorldEvent(Args&&... args)noexcept { m_worldEventQueue.Push(std::forward<Args>(args)...); }
   
    template<typename T>
    void AppendToWorldSendBuffer(T&& pkt_)noexcept { m_cur_send_buffer->Append<T>(std::forward<T>(pkt_)); }
    void ReturnSendBufferToWorld(SendBuffer* const pBuff) { m_send_buff_pool.ReturnSendBuffer(pBuff); }


     void AddAllObjects(const S_ptr<Session>& session)noexcept;
 private:
 	const S_ptr<MCTilemap> m_tileMap;
 	const S_ptr<MCTerrainGenerator> m_terrainGenerator;

    std::vector<S_ptr<Object>> m_worldObjects[static_cast<int>(MC_OBJECT_TYPE::END)];
    std::unordered_map<uint64_t, S_ptr<Object>> m_mapWorldObjects;

    float m_accTimeForUpdateInterval = 0.f;
 	Timer m_timer;

    LFQueue<std::function<void(void)>> m_worldEventQueue;
    SendBuffer* m_cur_send_buffer = nullptr;
    SendBufferPool m_send_buff_pool;

    std::thread m_worldUpdateThread;
 };

 static inline const auto& GetTileMap()noexcept { return MCWorld::GetInst()->GetTileMap(); }
