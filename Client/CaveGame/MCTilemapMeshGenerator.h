#pragma once
#include "pch.h"
#include "MCTilemap.h"

class Mesh;

class MCTilemapMeshGenerator
{
public:
	void CreateMeshAll(shared_ptr<MCTilemap> tilemap)const noexcept;
	static shared_ptr<Mesh> CreateMeshFromChunk(const shared_ptr<MCTilemap>& tilemap, int chunkX, int chunkZ, int textureID) noexcept;
	static void AddPlaneGreedyMesh(int map[][MCTileChunk::CHUNK_WIDTH], int mapWidth, int mapHeight, function<void(int, int, int, int)>&& vertexAddCallback, glm::vec3 normal, vector<glm::vec3>& vertices, vector<GLuint>& triangles, vector<glm::vec3>& normals, vector<glm::vec2>& uvs)noexcept;
};

