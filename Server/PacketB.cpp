#include "pch.h"
#include "PacketBase.hpp"
#include "pkbase.hpp"
#include "IOExecutor.h"
#include "Session.h"
#include "MCWorld.h"
#include "MCTilemap.h"

// c2s를 정의하는 CPP

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept

DECLARE_PACKET_FUNC(c2s_LOGIN)
{
	s2c_LOGIN pkt;
	pkt.mc_seed = MCWorld::G_MC_SEED;       // 월드의 시드 값 전달
	pkt.id = (uint32)id;                    // 클라이언트 고유 ID
	Mgr(IOExecutor)->GetSession(id)->ReserveSend(pkt); // 로그인 응답 패킷 전송
}

DECLARE_PACKET_FUNC(c2s_ENTER)
{
    glm::vec3 spawnPosition(256.0f, 16.0f, 256.0f); // 초기 위치 설정

    s2c_ENTER pkt;
    pkt.other_player_id = (uint32)id; // 입장한 클라이언트 ID 설정
    //pkt.spawn_x = spawnPosition.x;   // 초기 위치 X
    //pkt.spawn_y = spawnPosition.y;   // 초기 위치 Y
    //pkt.spawn_z = spawnPosition.z;   // 초기 위치 Z

    Mgr(IOExecutor)->AppendToSendBuffer(pkt); // 모든 클라이언트에 브로드캐스팅

    const auto session = Mgr(IOExecutor)->GetSession(id);
    for (const auto& [id_, otherSession] : Mgr(IOExecutor)->GetAllSessions())
    {
        if (id_ == id) continue;    // 자신은 제외
        pkt.other_player_id = (uint32)id_;    // 다른 클라이언트 ID 설정
        session->ReserveSend(pkt);           // 다른 클라이언트 정보를 현재 클라이언트에게 전송
    }

    session->SetMyGameObject(std::make_shared<Object>(session)); // 게임 객체 생성

    Mgr(MCWorld)->PostWorldEvent([session = std::move(session)]() {
        Mgr(MCWorld)->AddAllObjects(session); 
        });
}

DECLARE_PACKET_FUNC(c2s_DESTROY_BLOCK)
{
    s2c_DESTROY_BLOCK pkt;
    pkt.x = pkt_.x;
    pkt.y = pkt_.y;
    pkt.z = pkt_.z;

    // 타일 좌표 유효성 검증
    if (!Mgr(MCWorld)->GetTileMap()->IsValidPosition({ pkt_.x, pkt_.y, pkt_.z })) {
        LOG_ERROR("Invalid block position: (%d, %d, %d)", pkt_.x, pkt_.y, pkt_.z);
        return;
    }

    // 블록 파괴 이벤트 브로드캐스트
    Mgr(IOExecutor)->AppendToSendBuffer(pkt);

    // 블록 파괴 처리
    if (!Mgr(MCWorld)->GetTileMap()->SetTile({ pkt_.x, pkt_.y, pkt_.z }, 0)) {
        LOG_ERROR("Failed to destroy block at (%d, %d, %d)", pkt_.x, pkt_.y, pkt_.z);
        return;
    }

    LOG_INFO("Block destroyed at (%d, %d, %d) by player ID: %llu", pkt_.x, pkt_.y, pkt_.z, id);
}


DECLARE_PACKET_FUNC(c2s_KILL_PLAYER)
{
	const auto session = Mgr(IOExecutor)->GetSession(id);
	if (!session) return;

	s2c_KILL_PLAYER responsePkt;
    responsePkt.killer_id = (uint32)id;        // 플레이어 처치자 ID 설정
	responsePkt.target_id = pkt_.target_id;

    // 플레이어 사망 처리
    if (Mgr(MCWorld)->KillPlayer(id, pkt_.target_id)) {
        responsePkt.success = true;
        LOG_INFO("Player ID %u was killed by player ID %u.", pkt_.target_id, id);
    }
    else {
        responsePkt.success = false;
        LOG_ERROR("Failed to kill player ID %u by player ID %u.", pkt_.target_id, id);
    }

    // 모든 클라이언트에 브로드캐스트
    Mgr(IOExecutor)->AppendToSendBuffer(responsePkt);
    session->ReserveSend(responsePkt);
}

DECLARE_PACKET_FUNC(c2s_USE_ITEM)
{
    const auto session = Mgr(IOExecutor)->GetSession(id);
    if (!session) return;

    s2c_USE_ITEM responsePkt;
    responsePkt.user_id = (uint32)id;
    responsePkt.item_id = pkt_.item_id;

    // 아이템 사용 로직 처리
    //bool itemUsed = Mgr(MCWorld)->UseItem(id, pkt_.item_id, { pkt_.target_x, pkt_.target_y, pkt_.target_z });
    //responsePkt.success = itemUsed;

}

DECLARE_PACKET_FUNC(c2s_KILL_MONSTER)
{
    const auto session = Mgr(IOExecutor)->GetSession(id);
    if (!session) return;

    s2c_KILL_MONSTER responsePkt;
    responsePkt.killer_id = (uint32)id;          // 처치한 플레이어 ID 설정
    responsePkt.monster_id = pkt_.monster_id;   // 처치 대상 몬스터 ID 설정

    // 몬스터 사망 로직 처리
    //bool monsterKilled = Mgr(MCWorld)->KillMonster(pkt_.monster_id);
    //responsePkt.success = monsterKilled;        // 성공 여부 설정
    Mgr(IOExecutor)->AppendToSendBuffer(responsePkt);
    session->ReserveSend(responsePkt);
  
}
DECLARE_PACKET_FUNC(c2s_SPAWN_BOSS)
{
    const auto session = Mgr(IOExecutor)->GetSession(id);
    if (!session) return;

    s2c_SPAWN_BOSS responsePkt;
    responsePkt.boss_id = pkt_.boss_id;          // 보스 ID 설정
    responsePkt.position_x = pkt_.position_x;   // 소환 위치 X
    responsePkt.position_y = pkt_.position_y;   // 소환 위치 Y
    responsePkt.position_z = pkt_.position_z;   // 소환 위치 Z

    if (Mgr(MCWorld)->SpawnBoss(pkt_.boss_id, { pkt_.position_x, pkt_.position_y, pkt_.position_z })) {
        responsePkt.success = true;
        LOG_INFO("Boss ID %u spawned at (%f, %f, %f)", pkt_.boss_id, pkt_.position_x, pkt_.position_y, pkt_.position_z);
    }
    else {
        responsePkt.success = false;
        LOG_ERROR("Failed to spawn boss ID %u.", pkt_.boss_id);
    }

    Mgr(IOExecutor)->AppendToSendBuffer(responsePkt);
    session->ReserveSend(responsePkt);

 
}