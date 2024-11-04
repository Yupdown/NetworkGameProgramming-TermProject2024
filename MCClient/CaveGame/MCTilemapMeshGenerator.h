#pragma once
#include "pch.h"
#include "MCTilemap.h"
#include "ChunkRendererObject.h"

class Mesh;

class MCTilemapMeshGenerator
{
public:
	void CreateMeshAll(shared_ptr<MCTilemap> tilemap, array<array<shared_ptr<ChunkRendererObject>, MCTilemap::CHUNK_SIZE>, MCTilemap::CHUNK_SIZE>& pOut)const noexcept;
	static shared_ptr<Mesh> CreateMeshFromChunk(const shared_ptr<MCTilemap>& tilemap, int chunkX, int chunkZ) noexcept;
};