#include "pch.h"
#include "PacketBase.hpp"
#include "Hero.h"
#include "ServerObjectManager.h"
#include "ServerObjectFactory.h"

// Server -> Client , 서버로 부터의 패킷을 받아서 처리하는 함수들의 모임
// c2s는 없으면 링크에러나서 더미로 만들었음 좋은 의견있으면 건의 부탁

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
	if (Mgr(ServerObjectManager)->IsMyID(pkt_.other_player_id))
		return;

	ServerObjectBulider b;

	b.pos = G_INIT_POS;
	b.obj_id = pkt_.other_player_id;

	Mgr(ServerObjectManager)->AddObject(ServerObjectFactory::CreatePlayer(b), GROUP_TYPE::PLAYER);
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
	ServerObjectBulider b;
	b.obj_id = pkt_.object_id;
	b.pos = { pkt_.position_x,pkt_.position_y,pkt_.position_z };
	
	if (pkt_.obj_type == (uint8)MC_OBJECT_TYPE::MONSTER)
	{
		Mgr(ServerObjectManager)->AddObject(ServerObjectFactory::CreateMonster(b), GROUP_TYPE::MONSTER);
	}
	// TODO 다른 오브젝트
}

DECLARE_PACKET_FUNC(s2c_MOVE_OBJECT)
{
	if (Mgr(ServerObjectManager)->IsMyID(pkt_.object_id))
		return;

	if (const auto obj = Mgr(ServerObjectManager)->FindObject(pkt_.object_id))
	{
		obj->SyncMovement(pkt_);
	}
}

DECLARE_PACKET_FUNC(s2c_ADD_PROJECTILE)
{

}

DECLARE_PACKET_FUNC(s2c_USE_ITEM)
{

}