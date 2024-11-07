#include "pch.h"
#include "PacketBase.hpp"
#include "Hero.h"
// s2c를 정의 하는 CPP

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept        


extern shared_ptr<Hero> g_hero;

DECLARE_PACKET_FUNC(c2s_LOGIN)
{
}

DECLARE_PACKET_FUNC(s2c_LOGIN)
{

}

DECLARE_PACKET_FUNC(c2s_ENTER)
{
}

DECLARE_PACKET_FUNC(s2c_ENTER)
{
}

DECLARE_PACKET_FUNC(c2s_DESTROY_BLOCK)
{
}

DECLARE_PACKET_FUNC(s2c_DESTROY_BLOCK)
{
	g_hero->DestroyBlock({ pkt_.x,pkt_.y,pkt_.z });
}

DECLARE_PACKET_FUNC(c2s_CREATE_BLOCK)
{
}

DECLARE_PACKET_FUNC(s2c_CREATE_BLOCK)
{
	g_hero->CreateBlock({ pkt_.x,pkt_.y,pkt_.z }, pkt_.tile_id);
}
