#include "pch.h"
#include "ServerObjectFactory.h"
#include "Player.h"
#include "Hero.h"
#include "Monster.h"
#include "EnderDragon.h"
#include "MCTilemap.h"
#include "ProjectileArrow.h"
#include "ProjectileFireball.h"

shared_ptr<ServerObject> ServerObjectFactory::CreatePlayer(ServerObjectBulider& b_) noexcept
{
	const auto& b = static_cast<PlayerBuilder&>(b_);

	auto player = make_shared<Player>(g_tileMapForCreateObject);
	player->SetObjName("player");
	player->GetTransform()->SetLocalPosition(b.pos);
	player->SetRendererTexture(b.tex_id);
	player->SetID(b.obj_id);

	return player;
}

shared_ptr<ServerObject> ServerObjectFactory::CreateMonster(ServerObjectBulider& b_) noexcept
{
	auto mon = make_shared<Monster>(g_tileMapForCreateObject);
	mon->SetObjName("monster");
	mon->GetTransform()->SetLocalPosition(b_.pos);
	mon->SetID(b_.obj_id);

	return mon;
}

shared_ptr<ServerObject> ServerObjectFactory::CreateProjArrow(ServerObjectBulider& b_) noexcept
{
	const auto& b = static_cast<ProjBuilder&>(b_);

	auto projectile = make_shared<ProjectileArrow>(g_tileMapForCreateObject);

	glm::quat qrot = glm::quat(glm::vec3(glm::radians(b.rot_x), glm::radians(b.rot_y), 0.0f));
	const auto r = glm::rotate(qrot, glm::vec3(0.0f, 0.0f, 1.0f));

	projectile->SetID(b.obj_id);
	projectile->SetPosition(b.pos);
	projectile->SetVelocity(r * 32.0f);

	return projectile;
}

shared_ptr<ServerObject> ServerObjectFactory::CreateProjFireball(ServerObjectBulider& b_) noexcept
{
	const auto& b = static_cast<BossProjBuilder&>(b_);

	auto projectile = make_shared<ProjectileFireball>(g_tileMapForCreateObject);

	projectile->SetID(b.obj_id);
	projectile->SetPosition(b.pos);
	projectile->SetVelocity({ b.vel_x ,b.vel_y ,b.vel_z });

	return projectile;
}

shared_ptr<ServerObject> ServerObjectFactory::CreateEnderDragon(ServerObjectBulider& b_) noexcept
{
	auto b = static_cast<EnderDragonBuilder&>(b_);

	auto ed = std::make_shared<EnderDragon>();
	ed->GetTransform()->SetLocalPosition(glm::vec3(MCTilemap::MAP_WIDTH / 2, MCTilemap::MAP_HEIGHT, MCTilemap::MAP_WIDTH / 2));
	ed->SetID(b_.obj_id);
	// TODO 임시 테스트용

	return ed;
}
