#include "pch.h"
#include "PacketBase.hpp"
#include "Hero.h"
#include "ServerObjectManager.h"

// Server -> Client , ������ ������ ��Ŷ�� �޾Ƽ� ó���ϴ� �Լ����� ����
// c2s�� ������ ��ũ�������� ���̷� ������� ���� �ǰ������� ���� ��Ź

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept        

extern int G_MC_SEED;

DECLARE_PACKET_FUNC(s2c_LOGIN)
{
	G_MC_SEED = pkt_.mc_seed;
	Mgr(ServerObjectManager)->SetMyID(pkt_.id);
	std::cout << "Recv MC SEED !: " << G_MC_SEED << '\n';
}

DECLARE_PACKET_FUNC(s2c_ENTER)
{
	if (pkt_.other_player_id == Mgr(ServerObjectManager)->GetMyID())
		return;

	auto other_player = Mgr(ServerObjectManager)->CreatePlayer(pkt_.other_player_id);

	Mgr(ServerObjectManager)->AddObject(std::move(other_player), GROUP_TYPE::PLAYER);
}

DECLARE_PACKET_FUNC(s2c_DESTROY_BLOCK)
{
	Mgr(ServerObjectManager)->GetHero()->DestroyBlock({pkt_.x,pkt_.y,pkt_.z});
}


DECLARE_PACKET_FUNC(s2c_CREATE_BLOCK)
{
	Mgr(ServerObjectManager)->GetHero()->CreateBlock({ pkt_.x,pkt_.y,pkt_.z }, pkt_.tile_id);
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