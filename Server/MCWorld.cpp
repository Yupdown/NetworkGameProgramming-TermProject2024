#include "pch.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "MCTerrainGenerator.h"
#include "Object.h"
#include "c2s_PacketHandler.h"
#include "SendBuffer.h"
#include "IOExecutor.h"

MCWorld::MCWorld()
    : m_tileMap{ std::make_shared<MCTilemap>() }
    , m_terrainGenerator{ std::make_shared<MCTerrainGenerator>() }
{

}
 
 MCWorld::~MCWorld()
 {
     m_worldUpdateThread.join();
 }
 
 void MCWorld::Init() noexcept
 {
 	m_terrainGenerator->Generate(m_tileMap);
 	m_timer.Update();

    m_worldUpdateThread = std::thread{ [this]()noexcept {this->Update(); } };
 }
 
 void MCWorld::Update()
 {
     const auto io_executor = Mgr(IOExecutor);
     const bool& isRunning = io_executor->GetIsRunning();
     while(isRunning)
     {
         m_timer.Update();
         const auto dt = m_timer.GetDT();
         m_accTimeForUpdateInterval -= dt;
         if (0.f > m_accTimeForUpdateInterval)continue;
         m_accTimeForUpdateInterval = UPDATE_INTERVAL;
         //TODO: 업데이트 및, 종료플래그 받기

         if (nullptr == m_cur_send_buffer)
         {
             m_cur_send_buffer = m_send_buff_pool.GetSendBuffer();
         }

         for (int i = 0; i < etoi(MC_OBJECT_TYPE::END); ++i)
         {
             const auto& obj = m_worldObjects[i];
             auto b = obj.data();
             const auto e = b + obj.size();
             while (e != b) { (*b++)->Update(dt); }
         }

         if (0 == m_cur_send_buffer->GetLen())
         {
             // TODO: 아무것도 하지 않았다면 걍 냅둠
         }
         else
         {
             io_executor->PostWorldSendBuffer(m_cur_send_buffer);
             m_cur_send_buffer = nullptr;
         }
     }


 }

 const S_ptr<Object>& MCWorld::AddObject(S_ptr<Object> obj, const MC_OBJECT_TYPE eType) noexcept
 {
     m_mapWorldObjects.try_emplace(obj->GetObjectID(), obj);
     return m_worldObjects[static_cast<int>(eType)].emplace_back(std::move(obj));
 }
