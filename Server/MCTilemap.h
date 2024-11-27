#pragma once
#include "pch.h"

class Tile
{
public:
	static const int TEXTURES[][6];
	static const int TILE_OPAQUE[];
	constexpr static const char* TILE_PLACE_SOUND[] = {
		"stone4.ogg",
		"stone4.ogg",
		"gravel1.ogg",
		"grass1.ogg",
		"stone4.ogg",
		"wood1.ogg",
		"stone4.ogg",
		"wood1.ogg",
		"grass1.ogg",
		"stone4.ogg",
	};
	constexpr static const char* TILE_BREAK_SOUND[] = {
		"stone4.ogg",
		"stone4.ogg",
		"gravel1.ogg",
		"grass1.ogg",
		"stone4.ogg",
		"wood1.ogg",
		"stone4.ogg",
		"wood1.ogg",
		"grass1.ogg",
		"glass1.ogg",
	};
};

struct RaycastResult
{
	bool hit;
	glm::fvec3 hitPosition;
	glm::fvec3 hitNormal;
	glm::ivec3 hitTilePosition;
};

class MCTileChunk
{
public:
	static constexpr int CHUNK_WIDTH = 32;
	static constexpr int CHUNK_HEIGHT = 32;
	static constexpr int NUM_OF_TILES = CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT;
private:
	uint8_t tileData[CHUNK_WIDTH][CHUNK_HEIGHT][CHUNK_WIDTH];

public:
	MCTileChunk();

	void SetTile(int x, int y, int z, uint8_t tile);
	uint8_t GetTile(int x, int y, int z) const;
};

class MCTilemap
{
public:
	MCTilemap() = default;
	~MCTilemap() = default;

public:
	static constexpr int MAP_WIDTH = 128; // TODO: 원래 512였음
	static constexpr int MAP_HEIGHT = MCTileChunk::CHUNK_HEIGHT;
	static constexpr int CHUNK_SIZE = (MAP_WIDTH + (MCTileChunk::CHUNK_WIDTH - 1)) / MCTileChunk::CHUNK_WIDTH;
private:
	MCTileChunk tileChunk[CHUNK_SIZE][CHUNK_SIZE];
	std::bitset<MAP_WIDTH> m_tileFlag[MAP_WIDTH][MAP_HEIGHT];

	std::vector<std::function<void(MCTileChunk*, int, int)>> notifyCallback;
private:
	void SetTileFlag(const int32_t x, const int32_t y, const int32_t z,const bool flag)noexcept {
		m_tileFlag[x][y][z] = flag;
	}
public:
	const bool GetTileFlag(const int32_t x, const int32_t y, const int32_t z)const noexcept { return m_tileFlag[x][y][z]; }
	void SetTile(int x, int y, int z, uint8_t tile, bool notify = false) noexcept;
	void SetTile(const glm::ivec3& v, uint8_t tile, bool notify = false) noexcept;
	uint8_t GetTile(int x, int y, int z) const noexcept;
	uint8_t GetTile(const glm::ivec3& v) const noexcept;
	MCTileChunk*const GetChunk(int x, int z)noexcept;
	bool HandleCollision(const glm::vec3& pre_position, glm::vec3& position, glm::vec3& velocity)const noexcept;
	RaycastResult RaycastTile(const glm::vec3& start_position, const glm::vec3& direction, float distance) const noexcept;
	void AddNotifyCallback(const std::function<void(MCTileChunk*, int, int)>& callback)noexcept;
};