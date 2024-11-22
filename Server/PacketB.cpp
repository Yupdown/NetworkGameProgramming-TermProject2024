#include "pch.h"
#include "PacketBase.hpp"
#include "pkbase.hpp"
#include "IOExecutor.h"
#include "Session.h"
#include "MCWorld.h"
#include "MCTilemap.h"

// c2s�� �����ϴ� CPP

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept

DECLARE_PACKET_FUNC(c2s_LOGIN)
{
	s2c_LOGIN pkt;
	pkt.mc_seed = MCWorld::G_MC_SEED;       // ������ �õ� �� ����
	pkt.id = (uint32)id;                    // Ŭ���̾�Ʈ ���� ID
	Mgr(IOExecutor)->GetSession(id)->ReserveSend(pkt); // �α��� ���� ��Ŷ ����
}

DECLARE_PACKET_FUNC(c2s_ENTER)
{
    glm::vec3 spawnPosition(256.0f, 16.0f, 256.0f); // �ʱ� ��ġ ����

    s2c_ENTER pkt;
    pkt.other_player_id = (uint32)id; // ������ Ŭ���̾�Ʈ ID ����
    //pkt.spawn_x = spawnPosition.x;   // �ʱ� ��ġ X
    //pkt.spawn_y = spawnPosition.y;   // �ʱ� ��ġ Y
    //pkt.spawn_z = spawnPosition.z;   // �ʱ� ��ġ Z

    Mgr(IOExecutor)->AppendToSendBuffer(pkt); // ��� Ŭ���̾�Ʈ�� ��ε�ĳ����

    const auto session = Mgr(IOExecutor)->GetSession(id);
    for (const auto& [id_, otherSession] : Mgr(IOExecutor)->GetAllSessions())
    {
        if (id_ == id) continue;    // �ڽ��� ����
        pkt.other_player_id = (uint32)id_;    // �ٸ� Ŭ���̾�Ʈ ID ����
        session->ReserveSend(pkt);           // �ٸ� Ŭ���̾�Ʈ ������ ���� Ŭ���̾�Ʈ���� ����
    }

    session->SetMyGameObject(std::make_shared<Object>(session)); // ���� ��ü ����

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

    // Ÿ�� ��ǥ ��ȿ�� ����
    if (!Mgr(MCWorld)->GetTileMap()->IsValidPosition({ pkt_.x, pkt_.y, pkt_.z })) {
        LOG_ERROR("Invalid block position: (%d, %d, %d)", pkt_.x, pkt_.y, pkt_.z);
        return;
    }

    // ��� �ı� �̺�Ʈ ��ε�ĳ��Ʈ
    Mgr(IOExecutor)->AppendToSendBuffer(pkt);

    // ��� �ı� ó��
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
    responsePkt.killer_id = (uint32)id;        // �÷��̾� óġ�� ID ����
	responsePkt.target_id = pkt_.target_id;

    // �÷��̾� ��� ó��
    if (Mgr(MCWorld)->KillPlayer(id, pkt_.target_id)) {
        responsePkt.success = true;
        LOG_INFO("Player ID %u was killed by player ID %u.", pkt_.target_id, id);
    }
    else {
        responsePkt.success = false;
        LOG_ERROR("Failed to kill player ID %u by player ID %u.", pkt_.target_id, id);
    }

    // ��� Ŭ���̾�Ʈ�� ��ε�ĳ��Ʈ
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

    // ������ ��� ���� ó��
    //bool itemUsed = Mgr(MCWorld)->UseItem(id, pkt_.item_id, { pkt_.target_x, pkt_.target_y, pkt_.target_z });
    //responsePkt.success = itemUsed;

}

DECLARE_PACKET_FUNC(c2s_KILL_MONSTER)
{
    const auto session = Mgr(IOExecutor)->GetSession(id);
    if (!session) return;

    s2c_KILL_MONSTER responsePkt;
    responsePkt.killer_id = (uint32)id;          // óġ�� �÷��̾� ID ����
    responsePkt.monster_id = pkt_.monster_id;   // óġ ��� ���� ID ����

    // ���� ��� ���� ó��
    //bool monsterKilled = Mgr(MCWorld)->KillMonster(pkt_.monster_id);
    //responsePkt.success = monsterKilled;        // ���� ���� ����
    Mgr(IOExecutor)->AppendToSendBuffer(responsePkt);
    session->ReserveSend(responsePkt);
  
}
DECLARE_PACKET_FUNC(c2s_SPAWN_BOSS)
{
    const auto session = Mgr(IOExecutor)->GetSession(id);
    if (!session) return;

    s2c_SPAWN_BOSS responsePkt;
    responsePkt.boss_id = pkt_.boss_id;          // ���� ID ����
    responsePkt.position_x = pkt_.position_x;   // ��ȯ ��ġ X
    responsePkt.position_y = pkt_.position_y;   // ��ȯ ��ġ Y
    responsePkt.position_z = pkt_.position_z;   // ��ȯ ��ġ Z

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