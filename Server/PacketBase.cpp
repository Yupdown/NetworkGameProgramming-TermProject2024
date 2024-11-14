#include "pch.h"
#include "PacketBase.hpp"
#include "IOExecutor.h"
#include "Session.h"
#include "MCWorld.h"

// c2s를 정의하는 CPP

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept

DECLARE_PACKET_FUNC(c2s_LOGIN)
{
	s2c_LOGIN pkt;
	pkt.mc_seed = MCWorld::G_MC_SEED;
	Mgr(IOExecutor)->GetSession(id)->SendDirect(pkt);
}

DECLARE_PACKET_FUNC(c2s_ENTER)
{
}

DECLARE_PACKET_FUNC(c2s_DESTROY_BLOCK)
{
	s2c_DESTROY_BLOCK pkt;
	pkt.x = pkt_.x;
	pkt.y = pkt_.y;
	pkt.z = pkt_.z;
	Mgr(IOExecutor)->GetSession(id)->SendDirect(pkt);
	// TODO: 모든 유저에게 브로드캐스팅
}

DECLARE_PACKET_FUNC(c2s_CREATE_BLOCK)
{
	s2c_CREATE_BLOCK pkt;
	pkt.x = pkt_.x;
	pkt.y = pkt_.y;
	pkt.z = pkt_.z;
	pkt.tile_id = pkt_.tile_id;
	Mgr(IOExecutor)->GetSession(id)->SendDirect(pkt);
	// TODO: 모든 유저에게 브로드캐스팅
}

DECLARE_PACKET_FUNC(c2s_ADD_OBJECT)
{
}

DECLARE_PACKET_FUNC(c2s_MOVE_OBJECT)
{
}

DECLARE_PACKET_FUNC(c2s_ADD_PROJECTILE)
{
}