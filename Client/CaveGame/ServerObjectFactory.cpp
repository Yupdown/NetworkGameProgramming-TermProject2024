#include "pch.h"
#include "ServerObjectFactory.h"
#include "Player.h"
#include "Hero.h"
#include "Monster.h"

shared_ptr<ServerObject> ServerObjectFactory::CreatePlayer(ServerObjectBulider& b_) noexcept
{
	auto player = make_shared<Player>(g_tileMapForCreateObject);
	player->SetObjName("player");
	player->GetTransform()->SetLocalPosition(b_.pos);
	player->SetRendererTexture(b_.obj_id);
	player->SetID(b_.obj_id);

	return player;
}

shared_ptr<ServerObject> ServerObjectFactory::CreateMonster(ServerObjectBulider& b_) noexcept
{
	return shared_ptr<ServerObject>();
}
