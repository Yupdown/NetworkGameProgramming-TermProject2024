#include "pch.h"
#include "PacketBase.hpp"
#include "IOExecutor.h"
#include "Session.h"

// c2s�� �����ϴ� CPP

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept

DECLARE_PACKET_FUNC(c2s_LOGIN)
{
	std::cout << "��Ŷ!";
	Mgr(IOExecutor)->GetSession(id)->SendDirect(s2c_LOGIN{});
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
	// TODO: ��� �������� ��ε�ĳ����
}

DECLARE_PACKET_FUNC(c2s_CREATE_BLOCK)
{
	s2c_CREATE_BLOCK pkt;
	pkt.x = pkt_.x;
	pkt.y = pkt_.y;
	pkt.z = pkt_.z;
	pkt.tile_id = pkt_.tile_id;
	Mgr(IOExecutor)->GetSession(id)->SendDirect(pkt);
	// TODO: ��� �������� ��ε�ĳ����
}

