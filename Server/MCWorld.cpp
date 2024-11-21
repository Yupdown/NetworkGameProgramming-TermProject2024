#include "pch.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "MCTerrainGenerator.h"
#include "Object.h"
#include "c2s_PacketHandler.h"
#include "SendBuffer.h"
#include "IOExecutor.h"
#include "MCObjectFactory.h"
#include "Session.h"
#include "Component.h"

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
    m_cur_send_buffer = m_send_buff_pool.GetSendBuffer();
    MCObjectBuilder b;
    static constexpr glm::vec3 G_INIT_POS = glm::vec3(256.0f, 16.0f, 256.0f);
    for (int i = 0; i < G_NUM_OF_MONSTERS; ++i)
    {
        const float dx = static_cast<float>(rand() % 16 * 8 - 64);
        const float dz = static_cast<float>(rand() % 16 * 8 - 64);
        b.pos = glm::vec3(MCTilemap::MAP_WIDTH / 2 + dx, 32.0f, MCTilemap::MAP_WIDTH / 2 + dz);
       
        const auto& mon = AddObject(MCObjectFactory::CreateMonster(b), MC_OBJECT_TYPE::MONSTER);
        b.obj_id = mon->GetObjectID();

        mon->Init();
    }

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
         if (0.f < m_accTimeForUpdateInterval)continue;
         m_accTimeForUpdateInterval = UPDATE_INTERVAL;
         //TODO: 업데이트 및, 종료플래그 받기

         if (nullptr == m_cur_send_buffer)
         {
             m_cur_send_buffer = m_send_buff_pool.GetSendBuffer();
         }
         for (const auto& player : m_worldObjects[etoi(MC_OBJECT_TYPE::PLAYER)])
         {
             player->GetSession()->RegisterSendBuffer();
         }
         while (const auto world_event = m_worldEventQueue.Pop())
         {
             world_event->operator()();
             delete world_event;
         }
         for (int i = 1; i < etoi(MC_OBJECT_TYPE::END); ++i)
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

         for (const auto& player : m_worldObjects[etoi(MC_OBJECT_TYPE::PLAYER)])
         {
             const auto& session = player->GetSession();
             const auto send_buff = session->GetSendBuffer();
             if (0 == send_buff->GetLen())continue;
             io_executor->PostSendQueue(session->GetSessionID(), send_buff);
             session->ResetSendBuffer();
         }
     }


 }

 const S_ptr<Object>& MCWorld::AddObject(S_ptr<Object> obj, const MC_OBJECT_TYPE eType) noexcept
 {
     m_mapWorldObjects.try_emplace(obj->GetObjectID(), obj);
     return m_worldObjects[static_cast<int>(eType)].emplace_back(std::move(obj));
 }

 void MCWorld::AddAllObjects(const S_ptr<Session>& session) noexcept
 {
     const auto iter = m_mapWorldObjects.try_emplace(session->GetSessionID(), std::make_shared<Object>(session));

     if (!iter.second)return;
    
     m_worldObjects[etoi(MC_OBJECT_TYPE::PLAYER)].emplace_back(std::move(iter.first->second));

     session->RegisterSendBuffer();
     
     for (const auto& mon : m_worldObjects[etoi(MC_OBJECT_TYPE::MONSTER)])
     {
         const auto pos = mon->GetPos();

         s2c_ADD_OBJECT p;
         p.object_id = mon->GetObjectID();

         p.position_x = pos.x;
         p.position_y = pos.y;
         p.position_z = pos.z;

         p.obj_type = (uint8)MC_OBJECT_TYPE::MONSTER;
         
         session->GetSendBuffer()->Append(p);
     }
 }
