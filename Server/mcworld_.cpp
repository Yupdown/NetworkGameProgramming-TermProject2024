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
    // 쓰레드 종료 플래그 설정
    auto io_executor = Mgr(IOExecutor);
    if (io_executor) {};
    if (m_worldUpdateThread.joinable()) {
        m_worldUpdateThread.join();
    }
}

void MCWorld::Init() noexcept
{
    m_terrainGenerator->Generate(m_tileMap);
    m_timer.Update();
    m_cur_send_buffer = m_send_buff_pool.GetSendBuffer();
    MCObjectBuilder b;
    static constexpr glm::vec3 G_INIT_POS = glm::vec3(256.0f, 16.0f, 256.0f);

    // 일반 몬스터 생성
    for (int i = 0; i < G_NUM_OF_MONSTERS; ++i)
    {
        const float dx = static_cast<float>(rand() % 16 * 8 - 64);
        const float dz = static_cast<float>(rand() % 16 * 8 - 64);
        b.pos = glm::vec3(MCTilemap::MAP_WIDTH / 2 + dx, 32.0f, MCTilemap::MAP_WIDTH / 2 + dz);

        const auto& mon = AddObject(MCObjectFactory::CreateMonster(b), MC_OBJECT_TYPE::MONSTER);
        b.obj_id = mon->GetObjectID();

        mon->Init();
    }

    // 보스 몬스터 생성
    SpawnBoss(DEFAULT_BOSS_POSITION);

    m_worldUpdateThread = std::thread{ [this]()noexcept {this->Update(); } };
}
void MCWorld::Update()
{
    const auto io_executor = Mgr(IOExecutor);
    const bool& isRunning = io_executor->GetIsRunning();

    while (isRunning)
    {
        m_timer.Update();
        const auto dt = m_timer.GetDT();
        m_accTimeForUpdateInterval -= dt;

        if (m_accTimeForUpdateInterval > 0.f) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        m_accTimeForUpdateInterval = UPDATE_INTERVAL;

        if (!m_cur_send_buffer) {
            m_cur_send_buffer = m_send_buff_pool.GetSendBuffer();
        }

        // 1. 플레이어 객체 업데이트 및 유효성 검사
        auto& players = m_worldObjects[etoi(MC_OBJECT_TYPE::PLAYER)];
        const auto playerBegin = players.data();

        for (auto iter = playerBegin; iter != playerBegin + players.size();)
        {
            auto& player = *iter;

            if (player && player->IsValid()) {
                player->GetSession()->RegisterSendBuffer(); // 세션 전송 버퍼 등록
                ++iter;
            }
            else {
                if (player) {
                    m_mapWorldObjects.erase(player->GetObjectID());
                }
                std::swap(player, *((playerBegin - 1) + players.size()));
                players.pop_back();
            }
        }

        // 2. 월드 이벤트 처리
        while (const auto world_event = m_worldEventQueue.Pop())
        {
            if (world_event) {
                (*world_event)(); // 이벤트 실행
                delete world_event; // 메모리 해제
            }
        }

        // 3. 다른 객체 업데이트 및 유효성 검사
        for (int i = 1; i < etoi(MC_OBJECT_TYPE::END); ++i)
        {
            auto& objs = m_worldObjects[i];
            const auto objBegin = objs.data();

            for (auto iter = objBegin; iter != objBegin + objs.size();)
            {
                auto& obj = *iter;

                if (obj && obj->IsValid()) {
                    obj->Update(dt); // 객체 업데이트
                    ++iter;
                }
                else {
                    if (obj) {
                        m_mapWorldObjects.erase(obj->GetObjectID());
                    }
                    std::swap(obj, *((objBegin - 1) + objs.size()));
                    objs.pop_back();
                }
            }
        }

        // 4. 월드 전송 버퍼 전송
        if (m_cur_send_buffer && m_cur_send_buffer->GetLen() > 0)
        {
            io_executor->PostWorldSendBuffer(m_cur_send_buffer);
            m_cur_send_buffer = nullptr;
        }

        // 5. 플레이어 데이터 전송
        for (const auto& player : m_worldObjects[etoi(MC_OBJECT_TYPE::PLAYER)])
        {
            if (!player) continue;

            const auto& session = player->GetSession();
            if (!session) continue;

            const auto send_buff = session->GetSendBuffer();
            if (send_buff && send_buff->GetLen() > 0) {
                io_executor->PostSendQueue(session->GetSessionID(), send_buff);
                session->ResetSendBuffer();
            }
        }
    }
}