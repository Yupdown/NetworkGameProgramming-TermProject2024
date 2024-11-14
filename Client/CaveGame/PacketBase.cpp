#include "pch.h"
#include "PacketBase.hpp"
#include "Hero.h"
// Server -> Client , ������ ������ ��Ŷ�� �޾Ƽ� ó���ϴ� �Լ����� ����
// c2s�� ������ ��ũ�������� ���̷� ������� ���� �ǰ������� ���� ��Ź

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept        


extern shared_ptr<Hero> g_hero;
extern int G_MC_SEED;

DECLARE_PACKET_FUNC(s2c_LOGIN)
{
	G_MC_SEED = pkt_.mc_seed;
	std::cout << "Recv MC SEED !: " << G_MC_SEED << '\n';
}

DECLARE_PACKET_FUNC(s2c_ENTER)
{
}

DECLARE_PACKET_FUNC(s2c_DESTROY_BLOCK)
{
	g_hero->DestroyBlock({ pkt_.x,pkt_.y,pkt_.z });
}


DECLARE_PACKET_FUNC(s2c_CREATE_BLOCK)
{
	g_hero->CreateBlock({ pkt_.x,pkt_.y,pkt_.z }, pkt_.tile_id);
}

DECLARE_PACKET_FUNC(s2c_ADD_OBJECT)
{

}

DECLARE_PACKET_FUNC(s2c_MOVE_OBJECT)
{

}

DECLARE_PACKET_FUNC(s2c_ADD_PROJECTILE)
{

}