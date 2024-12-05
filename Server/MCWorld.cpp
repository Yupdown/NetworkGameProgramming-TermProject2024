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
    if (m_worldUpdateThread.joinable())
        m_worldUpdateThread.join();
}

void MCWorld::Init() noexcept
{
    m_terrainGenerator->Generate(m_tileMap);
    m_timerForUpdateLoopCheck.Update();
    m_timerForUpdate.Update();

    m_cur_send_buffer = m_send_buff_pool.GetSendBuffer();
    MCObjectBuilder b;
    static constexpr glm::vec3 G_INIT_POS = glm::vec3(256.0f, 16.0f, 256.0f);
    for (int i = 0; i < G_NUM_OF_MONSTERS; ++i)
    {
        const float dx = static_cast<float>(rand() % 16 * 8 - 64);
        const float dz = static_cast<float>(rand() % 16 * 8 - 64);
        b.pos = glm::vec3(MCTilemap::MAP_WIDTH / 2 + dx, 32.0f, MCTilemap::MAP_WIDTH / 2 + dz);

        const auto& mon = AddObject(MCObjectFactory::CreateMonster(b), MC_OBJECT_TYPE::MONSTER);
        
        mon->Init();
    }

    for (int i = 0; i < 100; ++i)
    {
        MCItemBuilder b;

        const float dx = static_cast<float>(rand() % 16 * 8 - 64);
        const float dz = static_cast<float>(rand() % 16 * 8 - 64);
        b.pos = glm::vec3(MCTilemap::MAP_WIDTH / 2 + dx, 16.0f, MCTilemap::MAP_WIDTH / 2 + dz);

        const auto& item = AddObject(MCObjectFactory::CreateItem(b), MC_OBJECT_TYPE::ITEM);
        
        item->Init();
    }

    m_worldUpdateThread = std::thread{ [this]()noexcept {this->Update(); } };
}

void MCWorld::Update()
{
    const auto io_executor = Mgr(IOExecutor);
    const bool& isRunning = io_executor->GetIsRunning();
    while (isRunning)
    {
        m_timerForUpdateLoopCheck.Update();
        const auto dt = m_timerForUpdateLoopCheck.GetDT();
        m_accTimeForUpdateInterval -= dt;
        if (0.f < m_accTimeForUpdateInterval && m_worldEventQueue.empty())continue;
        m_accTimeForUpdateInterval = UPDATE_INTERVAL;
        m_timerForUpdate.Update();
        const auto world_dt = m_timerForUpdate.GetDT();

        if (nullptr == m_cur_send_buffer)
        {
            m_cur_send_buffer = m_send_buff_pool.GetSendBuffer();
        }

        {
            auto& players = m_worldObjects[etoi(MC_OBJECT_TYPE::PLAYER)];
            const auto b = players.data();
            for (auto iter = b; iter != b + players.size();)
            {
                auto& player = *iter;
                if (player->IsValid())
                {
                    player->flag = true;
                    player->GetSession()->RegisterSendBuffer();
                    ++iter;
                }
                else
                {
                    m_mapWorldObjects.erase(player->GetObjectID());
                    std::swap(player, *((b - 1) + players.size()));
                    players.pop_back();
                }
            }
        }

        while (const auto world_event = m_worldEventQueue.Pop())
        {
            world_event->operator()();
            delete world_event;
        }

        for (int i = 1; i < etoi(MC_OBJECT_TYPE::END); ++i)
        {
            auto& objs = m_worldObjects[i];
            const auto b = objs.data();
            for (auto iter = b; iter != b + objs.size();)
            {
                auto& obj = *iter;
                if (obj->IsValid())
                {
                    obj->Update(world_dt);
                    ++iter;
                }
                else
                {
                    m_mapWorldObjects.erase(obj->GetObjectID());
                    std::swap(obj, *((b - 1) + objs.size()));
                    objs.pop_back();
                }
            }
        }

        if (m_worldObjects[etoi(MC_OBJECT_TYPE::PLAYER)].empty())
        {
            m_cur_send_buffer->Clear();
        }
        else
        {
            if (0 != m_cur_send_buffer->GetLen())
            {
                io_executor->PostWorldSendBuffer(m_cur_send_buffer);
                m_cur_send_buffer = nullptr;
            }
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
    obj->Init();
    m_mapWorldObjects.try_emplace(obj->GetObjectID(), obj);
    return m_worldObjects[static_cast<int>(eType)].emplace_back(std::move(obj));
}

void MCWorld::AddAllObjects(const S_ptr<Session>& session) noexcept
{
    const auto& obj = session->GetMyGameObject();
    const auto iter = m_mapWorldObjects.try_emplace(session->GetSessionID(), obj);

    if (!iter.second)return;
    obj->SetObjectType(MC_OBJECT_TYPE::PLAYER);
    m_worldObjects[etoi(MC_OBJECT_TYPE::PLAYER)].emplace_back(obj);

    session->RegisterSendBuffer();

    s2c_ADD_OBJECT p;

    for (const auto& mon : m_worldObjects[etoi(MC_OBJECT_TYPE::MONSTER)])
    {
        const auto pos = mon->GetPos();

        p.object_id = mon->GetObjectID();

        p.position_x = pos.x;
        p.position_y = pos.y;
        p.position_z = pos.z;

        p.obj_type = (uint8)MC_OBJECT_TYPE::MONSTER;

        session->GetSendBuffer()->Append(p);
    }

    for (const auto& item : m_worldObjects[etoi(MC_OBJECT_TYPE::ITEM)])
    {
        s2c_ITEM_DROP p;

        const auto pos = item->GetPos();

        p.obj_id = item->GetObjectID();

        p.x = pos.x;
        p.y = pos.y;
        p.z = pos.z;

        // TODO 아이템 종류

        session->GetSendBuffer()->Append(p);
    }
}
