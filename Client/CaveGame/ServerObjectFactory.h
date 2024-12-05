#pragma once
#include "pch.h"

class ServerObject;
class MCTilemap;

struct ServerObjectBulider
{
	uint32_t obj_id;
	glm::vec3 pos;
};

struct PlayerBuilder
	:public ServerObjectBulider
{
	uint8_t tex_id;
};

struct ProjBuilder
	:public ServerObjectBulider
{
	float rot_x, rot_y;
};

struct EnderDragonBuilder
	:public ServerObjectBulider
{
	// TODO: 보스를 위해 필요한 정보들
};

struct BossProjBuilder
	:public ServerObjectBulider
{
	float vel_x, vel_y, vel_z;
};

class ServerObjectFactory
{
public:
	ServerObjectFactory() = delete;
	~ServerObjectFactory() = delete;
public:
	static void SetTileMap(std::shared_ptr<MCTilemap> tileMap_)noexcept { g_tileMapForCreateObject = std::move(tileMap_); }
public:

	static shared_ptr<ServerObject> CreatePlayer(ServerObjectBulider& b_)noexcept;
	static shared_ptr<ServerObject> CreateMonster(ServerObjectBulider& b_)noexcept;
	static shared_ptr<ServerObject> CreateProjArrow(ServerObjectBulider& b_)noexcept;
	static shared_ptr<ServerObject> CreateProjFireball(ServerObjectBulider& b_)noexcept;
	static shared_ptr<ServerObject> CreateEnderDragon(ServerObjectBulider& b_)noexcept;
private:
	static inline std::shared_ptr<MCTilemap> g_tileMapForCreateObject;
};

