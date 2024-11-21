#include "pch.h"
#include "PacketBase.hpp"
#include "IOExecutor.h"
#include "Session.h"
#include "MCWorld.h"
#include "MCTilemap.h"

// c2s를 정의하는 CPP

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept

DECLARE_PACKET_FUNC(c2s_LOGIN)
{
	s2c_LOGIN pkt;
	pkt.mc_seed = MCWorld::G_MC_SEED;
	pkt.id = (uint32)id;
	Mgr(IOExecutor)->GetSession(id)->ReserveSend(pkt);
}

DECLARE_PACKET_FUNC(c2s_ENTER)
{
	glm::vec3(256.0f, 16.0f, 256.0f);

	s2c_ENTER pkt;
	pkt.other_player_id = (uint32)id;
	Mgr(IOExecutor)->AppendToSendBuffer(pkt);

	const auto s = Mgr(IOExecutor)->GetSession(id);
	for (const auto& [id_, session] : Mgr(IOExecutor)->GetAllSessions())
	{
		if (id_ == id)continue;
		pkt.other_player_id = (uint32)id_;
		s->ReserveSend(pkt);
	}

	auto session = Mgr(IOExecutor)->GetSession(id);
	session->SetMyGameObject(std::make_shared<Object>(session));

	Mgr(MCWorld)->PostWorldEvent([session = std::move(session)]() {Mgr(MCWorld)->AddAllObjects(session); });
}

DECLARE_PACKET_FUNC(c2s_DESTROY_BLOCK)
{
	s2c_DESTROY_BLOCK pkt;
	pkt.x = pkt_.x;
	pkt.y = pkt_.y;
	pkt.z = pkt_.z;
	//Mgr(IOExecutor)->GetSession(id)->SendDirect(pkt);
	// TODO: 모든 유저에게 브로드캐스팅

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
	//Mgr(IOExecutor)->GetSession(id)->SendDirect(pkt);
	// TODO: 모든 유저에게 브로드캐스팅
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

	Mgr(IOExecutor)->AppendToSendBuffer(pkt);
}

DECLARE_PACKET_FUNC(c2s_ADD_PROJECTILE)
{

}

DECLARE_PACKET_FUNC(c2s_USE_ITEM)
{

}
