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

 static constexpr const int G_NUM_OF_MONSTERS = 20;

 class MCWorld
 	: public Singleton<MCWorld>
 {
 	friend class Singleton;
 	MCWorld();
 	~MCWorld();
 public:
     static constexpr const int G_MC_SEED = 1;
     static constexpr const float UPDATE_INTERVAL = 0.01f;
 public:


 public:
 	const auto& GetTileMap()const noexcept { return m_tileMap; }
 	void Init()noexcept;
 	
    auto& GetTileMap()noexcept { return m_tileMap; }

 	void Update();

    const S_ptr<Object>& AddObject(S_ptr<Object> obj, const MC_OBJECT_TYPE eType)noexcept;
    const S_ptr<Object>& AddDropItem(glm::vec3 pos, uint8_t item_id) noexcept;

    template<typename... Args>
    void PostWorldEvent(Args&&... args)noexcept { m_worldEventQueue.Push(std::forward<Args>(args)...); }
   
    template<typename T>
    void AppendToWorldSendBuffer(T&& pkt_)noexcept { m_cur_send_buffer->Append<T>(std::forward<T>(pkt_)); }
    void ReturnSendBufferToWorld(SendBuffer* const pBuff) { m_send_buff_pool.ReturnSendBuffer(pBuff); }


     void AddAllObjects(const S_ptr<Session>& session)noexcept;
     const auto& GetWorldObjects(const MC_OBJECT_TYPE eType)const noexcept { return m_worldObjects[static_cast<int>(eType)]; }
     Object* GetWorldObject(const uint64_t id_)const noexcept {
         const auto iter = m_mapWorldObjects.find(id_);
         return m_mapWorldObjects.cend() != iter ? iter->second.get() : nullptr;
     }
 private:
 	const S_ptr<MCTilemap> m_tileMap;
 	const S_ptr<MCTerrainGenerator> m_terrainGenerator;

    std::vector<S_ptr<Object>> m_worldObjects[static_cast<int>(MC_OBJECT_TYPE::END)];
    std::unordered_map<uint64_t, S_ptr<Object>> m_mapWorldObjects;

    float m_accTimeForUpdateInterval = 0.f;
 	Timer m_timerForUpdateLoopCheck;

    Timer m_timerForUpdate;

    LFQueue<std::function<void(void)>> m_worldEventQueue;
    SendBuffer* m_cur_send_buffer = nullptr;
    SendBufferPool m_send_buff_pool;

    std::thread m_worldUpdateThread;
 };

 static inline const auto& GetTileMap()noexcept { return MCWorld::GetInst()->GetTileMap(); }
