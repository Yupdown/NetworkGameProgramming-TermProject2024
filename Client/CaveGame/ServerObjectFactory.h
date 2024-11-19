#pragma once
#include "pch.h"

class ServerObject;
class MCTilemap;

struct ServerObjectBulider
{
	uint32_t obj_id;
	glm::vec3 pos;
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

private:
	static inline std::shared_ptr<MCTilemap> g_tileMapForCreateObject;
};

