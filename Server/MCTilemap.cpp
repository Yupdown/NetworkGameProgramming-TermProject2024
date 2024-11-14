#include "pch.h"
#include "MCTilemap.h"
#include "CustomMath.hpp"
#include "PerlinNoise.hpp"

constexpr int Tile::TEXTURES[][6] = {
	   0, 0, 0, 0, 0, 0,
	   2, 2, 2, 2, 2, 2,
	   3, 3, 3, 3, 3, 3,
	   1, 3, 4, 4, 4, 4,
	   7, 7, 7, 7, 7, 7,
	   5, 5, 5, 5, 5, 5,
	   6, 6, 6, 6, 6, 6,
	   10, 10, 9, 9, 9, 9,
	   12, 12, 12, 12, 12, 12,
	   11, 11, 11, 11, 11, 11,
};

constexpr int Tile::TILE_OPAQUE[] = {
	0, 1, 1, 1, 1, 1, 1, 1, 0, 0
};

MCTileChunk::MCTileChunk() : tileData()
{

}

void MCTileChunk::SetTile(int x, int y, int z, uint8_t tile)
{
	tileData[x][y][z] = tile;
}

uint8_t MCTileChunk::GetTile(int x, int y, int z) const
{
	return tileData[x][y][z];
}

void MCTilemap::SetTile(int x, int y, int z, uint8_t tile, bool notify) noexcept
{
	constexpr int modulo = MCTileChunk::CHUNK_WIDTH;
	const int chunkX = x / modulo;
	const int chunkZ = z / modulo;
	const int localX = x % modulo;
	const int localZ = z % modulo;
	MCTileChunk* const pChunk = &tileChunk[chunkX][chunkZ];
	pChunk->SetTile(localX, y, localZ, tile);

	if (notify)
	{
		for (const auto& callback : notifyCallback)
		{
			callback(pChunk, chunkX, chunkZ);
			if (localX <= 0 && chunkX > 0)
				callback(&tileChunk[chunkX - 1][chunkZ], chunkX - 1, chunkZ);
			if (localX >= MCTileChunk::CHUNK_WIDTH - 1 && chunkX < MCTilemap::CHUNK_SIZE - 1)
				callback(&tileChunk[chunkX + 1][chunkZ], chunkX + 1, chunkZ);
			if (localZ <= 0 && chunkZ > 0)
				callback(&tileChunk[chunkX][chunkZ - 1], chunkX, chunkZ - 1);
			if (localZ >= MCTileChunk::CHUNK_WIDTH - 1 && chunkZ < MCTilemap::CHUNK_SIZE - 1)
				callback(&tileChunk[chunkX][chunkZ + 1], chunkX, chunkZ + 1);
		}
	}
}

void MCTilemap::SetTile(const glm::ivec3& v, uint8_t tile, bool notify) noexcept
{
	SetTile(v.x, v.y, v.z, tile, notify);
}

uint8_t MCTilemap::GetTile(int x, int y, int z) const noexcept
{
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT || z < 0 || z >= MAP_WIDTH)
		return 0;
	constexpr int modulo = MCTileChunk::CHUNK_WIDTH;
	return tileChunk[x / modulo][z / modulo].GetTile(x % modulo, y, z % modulo);
}

uint8_t MCTilemap::GetTile(const glm::ivec3& v) const noexcept
{
	return GetTile(v.x, v.y, v.z);
}

MCTileChunk* const MCTilemap::GetChunk(int x, int z)noexcept
{
	return &(tileChunk[x][z]);
}

bool MCTilemap::HandleCollision(const glm::vec3& pre_position, glm::vec3& position, glm::vec3& velocity)const noexcept
{
	const glm::vec3 world_pos = position;
	const glm::vec3 world_vel = velocity;

	const float w = 0.75f;
	const float h = 1.75f;

	glm::vec3 pre_pos = pre_position;
	glm::vec3 post_pos = world_pos;
	glm::vec3 post_vel = world_vel;

	bool landed = false;
	bool collided = false;

	int x_min = FloorToInt(pre_pos.x - w * 0.5f);
	int x_max = CeilToInt(pre_pos.x + w * 0.5f) - 1;
	int y_min = FloorToInt(post_pos.y);
	int y_max = CeilToInt(post_pos.y + h) - 1;
	int z_min = FloorToInt(pre_pos.z - w * 0.5f);
	int z_max = CeilToInt(pre_pos.z + w * 0.5f) - 1;

	if (x_min >= 0 && x_max < MAP_WIDTH && y_min >= 0 && y_max < MAP_HEIGHT && z_min >= 0 && z_max < MAP_WIDTH)
	{
		bool flag = true;
		for (int x = x_min; x <= x_max && flag; ++x)
		{
			for (int z = z_min; z <= z_max && flag; ++z)
			{
				if (world_vel.y < 0.0f && GetTile(x, y_min, z) > 0)
				{
					post_pos.y = y_min + 1.0f;
					post_vel.y = 0.0f;

					landed = true;
					collided = true;
					flag = false;
				}
				else if (world_vel.y > 0.0f && GetTile(x, y_max, z) > 0)
				{
					post_pos.y = y_max - h;
					post_vel.y = 0.0f;

					collided = true;
					flag = false;
				}
			}
		}
	}

	if (world_pos.x - w * 0.5f < 0.0f)
	{
		post_pos.x = w * 0.5f;
		post_vel.x = 0.0f;
	}
	if (world_pos.x + w * 0.5f > MAP_WIDTH)
	{
		post_pos.x = MAP_WIDTH - w * 0.5f;
		post_vel.x = 0.0f;
	}

	x_min = FloorToInt(post_pos.x - w * 0.5f);
	x_max = CeilToInt(post_pos.x + w * 0.5f) - 1;
	y_min = FloorToInt(post_pos.y);
	y_max = CeilToInt(post_pos.y + h) - 1;
	z_min = FloorToInt(pre_pos.z - w * 0.5f);
	z_max = CeilToInt(pre_pos.z + w * 0.5f) - 1;

	if (x_min >= 0 && x_max < MAP_WIDTH && y_min >= 0 && y_max < MAP_HEIGHT && z_min >= 0 && z_max < MAP_WIDTH)
	{
		bool flag = true;
		for (int y = y_min; y <= y_max && flag; ++y)
		{
			for (int z = z_min; z <= z_max && flag; ++z)
			{
				if (world_vel.x < 0.0f && GetTile(x_min, y, z) > 0)
				{
					post_pos.x = x_min + 1.0f + w * 0.5f;
					post_vel.x = 0.0f;

					collided = true;
					flag = false;
				}
				else if (world_vel.x > 0.0f && GetTile(x_max, y, z) > 0)
				{
					post_pos.x = x_max - w * 0.5f;
					post_vel.x = 0.0f;

					collided = true;
					flag = false;
				}
			}
		}
	}

	if (world_pos.z - w * 0.5f < 0.0f)
	{
		post_pos.z = w * 0.5f;
		post_vel.z = 0.0f;
	}
	if (world_pos.z + w * 0.5f > MAP_WIDTH)
	{
		post_pos.z = MAP_WIDTH - w * 0.5f;
		post_vel.z = 0.0f;
	}

	x_min = FloorToInt(post_pos.x - w * 0.5f);
	x_max = CeilToInt(post_pos.x + w * 0.5f) - 1;
	y_min = FloorToInt(post_pos.y);
	y_max = CeilToInt(post_pos.y + h) - 1;
	z_min = FloorToInt(post_pos.z - w * 0.5f);
	z_max = CeilToInt(post_pos.z + w * 0.5f) - 1;

	if (x_min >= 0 && x_max < MAP_WIDTH && y_min >= 0 && y_max < MAP_HEIGHT && z_min >= 0 && z_max < MAP_WIDTH)
	{
		bool flag = true;
		for (int x = x_min; x <= x_max && flag; ++x)
		{
			for (int y = y_min; y <= y_max && flag; ++y)
			{
				if (world_vel.z < 0.0f && GetTile(x, y, z_min) > 0)
				{
 					post_pos.z = z_min + 1.0f + w * 0.5f;
					post_vel.z = 0.0f;

					collided = true;
					flag = false;
				}
				else if (world_vel.z > 0.0f && GetTile(x, y, z_max) > 0)
				{
					post_pos.z = z_max - w * 0.5f;
					post_vel.z = 0.0f;

					collided = true;
					flag = false;
				}
			}
		}
	}

	position = post_pos;
	velocity = post_vel;

	return landed;
}

RaycastResult MCTilemap::RaycastTile(const glm::vec3& start_position, const glm::vec3& direction, float distance) const noexcept
{
	RaycastResult result = RaycastResult{ false, glm::zero<glm::vec3>(), glm::zero<glm::vec3>(), glm::zero<glm::ivec3>() };
	glm::vec3 tracePosition = start_position;

	while (glm::distance2(start_position, tracePosition) <= distance * distance)
	{
		glm::vec3 intersectionDelta = glm::one<glm::vec3>() * 1e+9f;
		glm::vec3 faceDirection = glm::zero<glm::vec3>();

		for (int i = 0; i < 3; i++)
		{
			if (direction[i] == 0.0f)
				continue;

			const int nextStep = direction[i] > 0.0f ? (int)glm::floor(tracePosition[i]) + 1 : (int)glm::ceil(tracePosition[i]) - 1;
			const glm::vec3 v = direction / direction[i] * (nextStep - tracePosition[i]);

			if (glm::length2(v) >= glm::length2(intersectionDelta))
				continue;

			intersectionDelta = v;
			switch (i)
			{
			case 0:
				faceDirection = direction[0] < 0.0f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(-1.0f, 0.0f, 0.0f);
				break;

			case 1:
				faceDirection = direction[1] < 0.0f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, -1.0f, 0.0f);
				break;

			case 2:
				faceDirection = direction[2] < 0.0f ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.0f, 0.0f, -1.0f);
				break;
			}
		}

		tracePosition += intersectionDelta;

		glm::ivec3 tilePosition = glm::ivec3();
		for (int i = 0; i < 3; i++)
			tilePosition[i] = direction[i] > 0.0f ? (int)glm::floor(tracePosition[i]) : (int)glm::ceil(tracePosition[i]) - 1;

		if (tilePosition.x < 0 || tilePosition.x >= MCTilemap::MAP_WIDTH ||
			tilePosition.y < 0 || tilePosition.y >= MCTilemap::MAP_HEIGHT ||
			tilePosition.z < 0 || tilePosition.z >= MCTilemap::MAP_WIDTH)
			return result;

		const int tile = GetTile(tilePosition);
		if (tile > 0)
		{
			result = { true, tracePosition, faceDirection, tilePosition };
			return result;
		}
	}
	return result;
}

void MCTilemap::AddNotifyCallback(const std::function<void(MCTileChunk*, int, int)>& callback)noexcept
{
	notifyCallback.push_back(callback);
}