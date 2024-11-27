#include "pch.h"
#include "PacketBase.hpp"
#include "Hero.h"
#include "ServerObjectManager.h"
#include "ServerObjectFactory.h"
#include "DropItem.h"
#include "MCItemManager.h"
#include "ProjectileArrow.h"

// Server -> Client , 서버로 부터의 패킷을 받아서 처리하는 함수들의 모임
// c2s는 없으면 링크에러나서 더미로 만들었음 좋은 의견있으면 건의 부탁

#define DECLARE_PACKET_FUNC(pkt_name) void pkt_name::Handle(const uint64 id, const pkt_name& pkt_) noexcept        

extern int G_MC_SEED;

DECLARE_PACKET_FUNC(s2c_LOGIN)
{
	G_MC_SEED = pkt_.mc_seed;
	Mgr(ServerObjectManager)->SetMyID(pkt_.id);
	Mgr(ServerObjectManager)->SetMyTexID(pkt_.tex_id);
	std::cout << "Recv MC SEED !: " << G_MC_SEED << '\n';
}

DECLARE_PACKET_FUNC(s2c_ENTER)
{
	if (Mgr(ServerObjectManager)->IsMyID(pkt_.other_player_id))
		return;

	PlayerBuilder b;

	b.pos = G_INIT_POS;
	b.obj_id = pkt_.other_player_id;
	b.tex_id = pkt_.other_player_texture_id;

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
	else if (pkt_.obj_type == (uint8)MC_OBJECT_TYPE::BOSS)
	{
		// TODO 테스트용 하드코딩
		EnderDragonBuilder bb;
		bb.obj_id = b.obj_id;
		bb.pos = b.pos;
		Mgr(ServerObjectManager)->AddObject(ServerObjectFactory::CreateEnderDragon(b), GROUP_TYPE::MONSTER);
	}
	// TODO 다른 오브젝트
}

DECLARE_PACKET_FUNC(s2c_MOVE_OBJECT)
{
	if (Mgr(ServerObjectManager)->IsMyID(pkt_.object_id))
		return;
	
	if (const auto obj = Mgr(ServerObjectManager)->FindObject(pkt_.object_id))
	{
		// std::cout << pkt_.object_id << std::endl;
		obj->SyncMovement(pkt_);
	}
}

DECLARE_PACKET_FUNC(s2c_ADD_PROJECTILE)
{
	if (Mgr(ServerObjectManager)->IsMyID(pkt_.fire_player_id))
	{
		const auto& projs = Mgr(ServerObjectManager)->GetTargetScene()->GetGroupObj(GROUP_TYPE::PROJ_PLAYER);

		for (const auto& proj : projs)
		{
			const auto p = static_cast<ProjectileArrow*>(proj.get());
			if (p->GetArrowLocalID() == pkt_.shooter_local_arrow_id)
			{
				p->SetID(pkt_.projectile_id);
				break;
			}
		}
		
	}
	else if (const auto obj = Mgr(ServerObjectManager)->FindObject(pkt_.fire_player_id))
	{
		// std::cout << pkt_.object_id << std::endl;
		static_cast<Player*>(obj.get())->Fire({ pkt_.pos_x,pkt_.pos_y ,pkt_.pos_z }, pkt_.dir_x, pkt_.dir_y)->SetID(pkt_.projectile_id);
	}
}

DECLARE_PACKET_FUNC(s2c_USE_ITEM)
{

}

DECLARE_PACKET_FUNC(s2c_REMOVE_OBJECT)
{
	Mgr(ServerObjectManager)->RemoveObject(pkt_.object_id);
}

DECLARE_PACKET_FUNC(s2c_MON_ATK)
{
}

DECLARE_PACKET_FUNC(s2c_ITEM_DROP)
{
	// 아이템 종류, 수량 등 더 받아야할정보가 있음
	auto pDropItem = make_shared<DropItem>(GetTileMapGlobal(), Mgr(MCItemManager)->GetItemByID(10), 1);
	pDropItem->GetTransform()->SetLocalPosition({ pkt_.x,pkt_.y,pkt_.z });
	pDropItem->SetID(pkt_.obj_id);
	// 확인용
	pDropItem->GetTransform()->SetLocalScale(5.f);
	Mgr(ServerObjectManager)->AddObject(std::move(pDropItem), GROUP_TYPE::DROP_ITEM);
}

DECLARE_PACKET_FUNC(s2c_SUMMON_BOSS)
{
	EnderDragonBuilder b;
	b.obj_id = pkt_.boss_id;
	b.pos.x = pkt_.pos_x;
	b.pos.y = pkt_.pos_y;
	b.pos.z = pkt_.pos_z;
	Mgr(ServerObjectManager)->AddObject(ServerObjectFactory::CreateEnderDragon(b), GROUP_TYPE::MONSTER);
}
