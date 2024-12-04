#include "pch.h"
#include "PacketBase.hpp"
#include "Hero.h"
#include "ServerObjectManager.h"
#include "ServerObjectFactory.h"
#include "DropItem.h"
#include "MCItemManager.h"
#include "ProjectileArrow.h"
#include "EventMgr.h"
#include "DeathObject.h"

// Server -> Client , ������ ������ ��Ŷ�� �޾Ƽ� ó���ϴ� �Լ����� ����
// c2s�� ������ ��ũ�������� ���̷� ������� ���� �ǰ������� ���� ��Ź

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
		// TODO �׽�Ʈ�� �ϵ��ڵ�
		EnderDragonBuilder bb;
		bb.obj_id = b.obj_id;
		bb.pos = b.pos;
		Mgr(ServerObjectManager)->AddObject(ServerObjectFactory::CreateEnderDragon(b), GROUP_TYPE::MONSTER);
	}
	// TODO �ٸ� ������Ʈ
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
	ProjArrowBuilder b;
	b.obj_id = pkt_.projectile_id;
	b.pos.x = pkt_.pos_x;
	b.pos.y = pkt_.pos_y;
	b.pos.z = pkt_.pos_z;
	b.rot_x = pkt_.dir_x;
	b.rot_y = pkt_.dir_y;

	Mgr(ServerObjectManager)->AddObject(ServerObjectFactory::CreateProjArrow(b), GROUP_TYPE::PROJ_PLAYER);
}

DECLARE_PACKET_FUNC(s2c_USE_ITEM)
{

}

DECLARE_PACKET_FUNC(s2c_ITEM_DROP)
{
	// ������ ����, ���� �� �� �޾ƾ��������� ����
	auto pDropItem = make_shared<DropItem>(GetTileMapGlobal(), Mgr(MCItemManager)->GetItemByID(10), 1);
	pDropItem->GetTransform()->SetLocalPosition({ pkt_.x,pkt_.y,pkt_.z });
	pDropItem->SetID(pkt_.obj_id);
	// Ȯ�ο�
	// pDropItem->GetTransform()->SetLocalScale(5.f);
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

DECLARE_PACKET_FUNC(s2c_REMOVE_OBJECT)
{
	// TODO: ������� ������Ʈ Ÿ�Կ����� �ٸ��ൿ�ϱ�
	shared_ptr<ServerObject> obj = Mgr(ServerObjectManager)->FindObject(pkt_.object_id);
	if (obj == nullptr)
		return;

	// TODO: ������Ʈ���� ����� �� ���� �̺�Ʈ�� �ִٸ�
	const auto type = (MC_OBJECT_TYPE)pkt_.obj_type;
	switch (type)
	{
	case MC_OBJECT_TYPE::ARROW:
	{
		break;
	}
	case MC_OBJECT_TYPE::MONSTER:
	{
		const auto monster = static_cast<Player*>(obj.get());
		std::shared_ptr<DeathObject> deathObj = make_shared<DeathObject>(monster->GetRendererObj());
		deathObj->GetTransform()->SetLocalPosition(obj->GetPosition());
		CreateObj(std::move(deathObj), GROUP_TYPE::DEFAULT);
		break;
	}
	default:
		break;
	}

	Mgr(ServerObjectManager)->RemoveObject(pkt_.object_id);
}

DECLARE_PACKET_FUNC(s2c_PLAYER_HIT)
{
	const bool bIsDead = 0 >= pkt_.hit_after_hp;

	std::shared_ptr<ServerObject> target = {};

	if (Mgr(ServerObjectManager)->IsMyID(pkt_.hit_player_id))
	{
		std::cout << "�ƾ� !\n";
		target = Mgr(ServerObjectManager)->GetHero();
	}
	else if (const auto player = Mgr(ServerObjectManager)->FindObject(pkt_.hit_player_id))
	{
		target = player;
	}

	if (target)
	{
		target->OnObjectDamaged(pkt_.hit_player_id);
	}
}