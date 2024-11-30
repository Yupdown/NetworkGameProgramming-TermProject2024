#include "pch.h"
#include "PacketBase.hpp"
#include "IOExecutor.h"
#include "Session.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "Object.h"
#include "PathFollower.h"
#include "MCObjectFactory.h"
#include "HP.h"

// c2s를 정의하는 CPP

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept

DECLARE_PACKET_FUNC(c2s_LOGIN)
{
	const auto& session = Mgr(IOExecutor)->GetSession(id);
	s2c_LOGIN pkt;
	pkt.mc_seed = MCWorld::G_MC_SEED;
	pkt.id = (uint32)id;
	pkt.tex_id = session->GetPlayerTexID();
	session->ReserveSend(pkt);
}

DECLARE_PACKET_FUNC(c2s_ENTER)
{
	s2c_ENTER pkt;
	auto s = Mgr(IOExecutor)->GetSession(id);
	pkt.other_player_id = (uint32)id;
	pkt.other_player_texture_id = s->GetPlayerTexID();

	Mgr(IOExecutor)->AppendToSendBuffer(pkt);

	
	for (const auto& [id_, session] : Mgr(IOExecutor)->GetAllSessions())
	{
		if (id_ == id)continue;
		pkt.other_player_id = (uint32)id_;
		pkt.other_player_texture_id = session->GetPlayerTexID();

		s->ReserveSend(pkt);
	}

	s->SetMyGameObject(std::make_shared<Object>(s));

	Mgr(MCWorld)->PostWorldEvent([session = std::move(s)]() {Mgr(MCWorld)->AddAllObjects(session); });
}

DECLARE_PACKET_FUNC(c2s_DESTROY_BLOCK)
{
	s2c_DESTROY_BLOCK pkt;
	pkt.x = pkt_.x;
	pkt.y = pkt_.y;
	pkt.z = pkt_.z;
	
	Mgr(IOExecutor)->AppendToSendBuffer(pkt);
	Mgr(MCWorld)->GetTileMap()->SetTile({ pkt_.x ,pkt_.y ,pkt_.z }, 0);
}

DECLARE_PACKET_FUNC(c2s_CREATE_BLOCK)
{
	s2c_CREATE_BLOCK pkt;
	pkt.x = pkt_.x;
	pkt.y = pkt_.y;
	pkt.z = pkt_.z;
	pkt.tile_id = pkt_.tile_id;
	
	Mgr(IOExecutor)->AppendToSendBuffer(pkt);

	Mgr(MCWorld)->GetTileMap()->SetTile({ pkt_.x ,pkt_.y ,pkt_.z }, pkt_.tile_id);
}

DECLARE_PACKET_FUNC(c2s_ADD_OBJECT)
{
	
}

DECLARE_PACKET_FUNC(c2s_MOVE_OBJECT)
{
	s2c_MOVE_OBJECT pkt;
	pkt.object_id = (uint32)id;
	pkt.position_x = pkt_.position_x;
	pkt.position_y = pkt_.position_y;
	pkt.position_z = pkt_.position_z;

	pkt.acceleration_x = pkt_.acceleration_x;
	pkt.acceleration_y = pkt_.acceleration_y;
	pkt.acceleration_z = pkt_.acceleration_z;

	pkt.velocity_x = pkt_.velocity_x;
	pkt.velocity_y = pkt_.velocity_y;
	pkt.velocity_z = pkt_.velocity_z;

	pkt.pitch = pkt_.pitch;
	pkt.rotation_y = pkt_.rotation_y;
	pkt.yaw = pkt_.yaw;

	const auto& session = Mgr(IOExecutor)->GetSession(id);
	if (!session)return;
	const auto& player = session->GetMyGameObject();
	if (!player)return;
	const auto movement = &player->GetPosInfo();

	movement->m_vPos = { pkt_.position_x ,pkt_.position_y ,pkt_.position_z };
	movement->m_vVelocity = { pkt_.velocity_x ,pkt_.velocity_y ,pkt_.velocity_z };
	movement->m_vAccelation = { pkt_.acceleration_x ,pkt_.acceleration_y ,pkt_.acceleration_z };
	movement->m_cameraAngleAxisSmooth = { pkt_.cam_x,pkt_.cam_y ,pkt_.cam_z };
	
	Mgr(IOExecutor)->AppendToSendBuffer(pkt);
}

DECLARE_PACKET_FUNC(c2s_ADD_PROJECTILE)
{
	s2c_ADD_PROJECTILE pkt;
	
	pkt.dir_x = pkt_.dir_x;
	pkt.dir_y = pkt_.dir_y;

	pkt.pos_x = pkt_.pos_x;
	pkt.pos_y = pkt_.pos_y;
	pkt.pos_z = pkt_.pos_z;

	ProjArrowBuilder b;
	b.pos.x = pkt_.pos_x;
	b.pos.y = pkt_.pos_y;
	b.pos.z = pkt_.pos_z;
	b.rot_x = pkt_.dir_x;
	b.rot_y = pkt_.dir_y;

	auto a = MCObjectFactory::CreateProjArrow(b);

	pkt.projectile_id = a->GetObjectID();
	
	Mgr(MCWorld)->PostWorldEvent([arrow = std::move(a)]()mutable {Mgr(MCWorld)->AddObject(std::move(arrow), MC_OBJECT_TYPE::ARROW); });

	Mgr(IOExecutor)->AppendToSendBuffer(pkt);
}

DECLARE_PACKET_FUNC(c2s_USE_ITEM)
{

}

DECLARE_PACKET_FUNC(c2s_SUMMON_BOSS)
{
	// TODO: 드래곤소환후 월드에 넣기

	EnderDragonBuilder b;
	b.pos = glm::vec3(MCTilemap::MAP_WIDTH / 2, MCTilemap::MAP_HEIGHT - 10, MCTilemap::MAP_WIDTH / 2);

	auto ed = MCObjectFactory::CreateEnderDragon(b);

	s2c_SUMMON_BOSS pkt;
	pkt.boss_id = ed->GetObjectID();
	pkt.pos_x = b.pos.x;
	pkt.pos_y = b.pos.y;
	pkt.pos_z = b.pos.z;

	Mgr(MCWorld)->PostWorldEvent([ed = std::move(ed)]() {Mgr(MCWorld)->AddObject(ed, MC_OBJECT_TYPE::BOSS); });
	Mgr(IOExecutor)->AppendToSendBuffer(pkt);
}
