#include "pch.h"

#include "MCTilemapMeshGenerator.h"
#include "MCTilemap.h"

#include <Vertex.hpp>
#include <Mesh.h>
#include "MeshRenderer.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "GameObj.h"
#include "Material.h"
#include "Transform.h"
#include "EventMgr.h"
#include "ChunkMesh.h"
#include "Player.h"

static constexpr uint MAX_TEXTURE = 12;

extern std::atomic_bool g_bTileFinish;

thread_local int planeMap[MCTileChunk::CHUNK_WIDTH][MCTileChunk::CHUNK_WIDTH];

void MCTilemapMeshGenerator::CreateMeshAll(shared_ptr<MCTilemap> tilemap)const noexcept
{
    g_bTileFinish.store(false);

    shared_ptr<Material> material[MAX_TEXTURE];
    shared_ptr<ChunkMesh> pChunkDatas[MAX_TEXTURE];

    for (int i = 0; i < MAX_TEXTURE; ++i)
    {
        material[i] = make_shared<Material>();
        material[i]->AddTexture2D(std::format("tile_{}.png", i + 1));
        pChunkDatas[i] = make_shared<ChunkMesh>(tilemap);
        pChunkDatas[i]->SetChunkMaterial(material[i]);
        tilemap->AddNotifyCallback([pChunkDatas, i](MCTileChunk* pChunk, int chunkX, int chunkZ) noexcept{ pChunkDatas[i]->OnChunkMeshChanged(pChunk, chunkX, chunkZ); });
    }
    vector<std::future<void>> generate_mesh;
    generate_mesh.reserve(MCTilemap::CHUNK_SIZE * MCTilemap::CHUNK_SIZE * MAX_TEXTURE);
    for (int chunkX = 0; chunkX < MCTilemap::CHUNK_SIZE; ++chunkX)
    {
        for (int chunkZ = 0; chunkZ < MCTilemap::CHUNK_SIZE; ++chunkZ)
        {
            for (int textureID = 0; textureID < MAX_TEXTURE; ++textureID)
            {
                shared_ptr<GameObj> terrainObj = GameObj::make_obj();
                auto renderer = terrainObj->AddComponent<MeshRenderer>();
                generate_mesh.emplace_back(Mgr(ThreadMgr)->EnqueueTaskFuture([this, tilemap, chunkX, chunkZ, textureID, renderer]()noexcept {
                    shared_ptr<Mesh> mesh = this->CreateMeshFromChunk(tilemap, chunkX, chunkZ, textureID + 1);
                    renderer->AddMesh(mesh);
                    }));
                MCTileChunk* pChunk = tilemap->GetChunk(chunkX, chunkZ);
               
                terrainObj->GetTransform()->SetLocalScale(0.1f);
                terrainObj->GetTransform()->SetLocalPosition({ chunkX * MCTileChunk::CHUNK_WIDTH * 0.1f, 0.0f, chunkZ * MCTileChunk::CHUNK_WIDTH * 0.1f });
                
                pChunkDatas[textureID]->AddChunk(terrainObj,pChunk);
                pChunkDatas[textureID]->SetChunkMeshTexID(textureID + 1);
            }
        }
    }
    for (auto& f : generate_mesh)f.get();
    Mgr(EventMgr)->AddEventNeedLock([pChunkDatas]() noexcept
        {   
            const auto curScene = Mgr(SceneMgr)->GetCurScene();
            for (int i = 0; i < MAX_TEXTURE; ++i)
            {
                Mgr(ThreadMgr)->Enqueue(&ChunkMesh::MergeMeshData, pChunkDatas[i].get());
            }
            Mgr(ThreadMgr)->WaitAllJob();
            for (int i = 0; i < MAX_TEXTURE; ++i)
            {
                pChunkDatas[i]->InitChunkMesh("DefaultWarpShader.glsl");
                curScene->AddChunkMesh(i, pChunkDatas[i]);
            }
            static_pointer_cast<Player>(curScene->GetPlayer())->InitCamDirection();
            g_bTileFinish.store(true);
        });
}

thread_local vector<Vertex> sVertices;
thread_local GLuint vert_cnt = 0;
thread_local GLuint idx_cnt = 1024 * 128 * 2;

shared_ptr<Mesh> MCTilemapMeshGenerator::CreateMeshFromChunk(const shared_ptr<MCTilemap>& tilemap, int chunkX, int chunkZ, int textureID) noexcept
{
    const MCTileChunk* const chunk = tilemap->GetChunk(chunkX, chunkZ);
    const int offsetX = chunkX * MCTileChunk::CHUNK_WIDTH;
    const int offsetZ = chunkZ * MCTileChunk::CHUNK_WIDTH;

    vector<glm::vec3> vertices; vertices.reserve(4);
    vector<glm::vec3> normals; normals.reserve(4);
    vector<glm::vec2> uvs; uvs.reserve(4);
    vector<GLuint> triangles;triangles.reserve(idx_cnt + 1024);
   
    for (int y = 0; y < MCTileChunk::CHUNK_HEIGHT; y++)
    {
        for (int x = 0; x < MCTileChunk::CHUNK_WIDTH; x++)
        {
            for (int z = 0; z < MCTileChunk::CHUNK_WIDTH; z++)
                planeMap[x][z] = Tile::TEXTURES[tilemap->GetTile(x + offsetX, y, z + offsetZ)][0] == textureID ? (y < MCTilemap::MAP_HEIGHT - 1 ? (Tile::TILE_OPAQUE[tilemap->GetTile(x + offsetX, y + 1, z + offsetZ)] ? 0 : 1) : 1) : 0;
        }

        AddPlaneGreedyMesh(planeMap, MCTileChunk::CHUNK_WIDTH, MCTileChunk::CHUNK_WIDTH, [&vertices,y](int xmin, int ymin, int xmax, int ymax)noexcept
            {
                vertices.emplace_back(glm::vec3(xmin, y + 1, ymin));
                vertices.emplace_back(glm::vec3(xmax, y + 1, ymin));
                vertices.emplace_back(glm::vec3(xmin, y + 1, ymax));
                vertices.emplace_back(glm::vec3(xmax, y + 1, ymax));
            }, glm::vec3(0.0f, 1.0f, 0.0f), vertices, triangles, normals, uvs);
    }

    for (int y = 0; y < MCTileChunk::CHUNK_HEIGHT; y++)
    {
        for (int x = 0; x < MCTileChunk::CHUNK_WIDTH; x++)
        {
            for (int z = 0; z < MCTileChunk::CHUNK_WIDTH; z++)
                planeMap[x][z] = Tile::TEXTURES[tilemap->GetTile(x + offsetX, y, z + offsetZ)][1] == textureID ? (y > 0 ? (Tile::TILE_OPAQUE[tilemap->GetTile(x + offsetX, y - 1, z + offsetZ)] ? 0 : 1) : 1) : 0;
        }

        AddPlaneGreedyMesh(planeMap, MCTileChunk::CHUNK_WIDTH, MCTileChunk::CHUNK_WIDTH, [&vertices, y](int xmin, int ymin, int xmax, int ymax)noexcept
            {
                vertices.emplace_back(glm::vec3(xmax, y, ymin));
                vertices.emplace_back(glm::vec3(xmin, y, ymin));
                vertices.emplace_back(glm::vec3(xmax, y, ymax));
                vertices.emplace_back(glm::vec3(xmin, y, ymax));
            }, glm::vec3(0.0f, -1.0f, 0.0f), vertices, triangles, normals, uvs);
    }

    for (int x = 0; x < MCTileChunk::CHUNK_WIDTH; x++)
    {
        for (int y = 0; y < MCTileChunk::CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < MCTileChunk::CHUNK_WIDTH; z++)
                planeMap[z][y] = Tile::TEXTURES[tilemap->GetTile(x + offsetX, y, z + offsetZ)][2] == textureID ? (x + offsetX < MCTilemap::MAP_WIDTH - 1 ? (Tile::TILE_OPAQUE[tilemap->GetTile(x + offsetX + 1, y, z + offsetZ)] ? 0 : 1) : 1) : 0;
        }

        AddPlaneGreedyMesh(planeMap, MCTileChunk::CHUNK_WIDTH, MCTileChunk::CHUNK_HEIGHT, [&vertices, x](int xmin, int ymin, int xmax, int ymax)noexcept
            {
                vertices.emplace_back(glm::vec3(x + 1, ymin, xmin));
                vertices.emplace_back(glm::vec3(x + 1, ymin, xmax));
                vertices.emplace_back(glm::vec3(x + 1, ymax, xmin));
                vertices.emplace_back(glm::vec3(x + 1, ymax, xmax));
            }, glm::vec3(1.0f, 0.0f, 0.0f), vertices, triangles, normals, uvs);
    }

    for (int x = 0; x < MCTileChunk::CHUNK_WIDTH; x++)
    {
        for (int y = 0; y < MCTileChunk::CHUNK_HEIGHT; y++)
        {
            for (int z = 0; z < MCTileChunk::CHUNK_WIDTH; z++)
                planeMap[z][y] = Tile::TEXTURES[tilemap->GetTile(x + offsetX, y, z + offsetZ)][3] == textureID ? (x + offsetX > 0 ? (Tile::TILE_OPAQUE[tilemap->GetTile(x + offsetX - 1, y, z + offsetZ)] ? 0 : 1) : 1) : 0;
        }

        AddPlaneGreedyMesh(planeMap, MCTileChunk::CHUNK_WIDTH, MCTileChunk::CHUNK_HEIGHT, [&vertices, x](int xmin, int ymin, int xmax, int ymax)noexcept
            {
                vertices.emplace_back(glm::vec3(x, ymin, xmax));
                vertices.emplace_back(glm::vec3(x, ymin, xmin));
                vertices.emplace_back(glm::vec3(x, ymax, xmax));
                vertices.emplace_back(glm::vec3(x, ymax, xmin));
            }, glm::vec3(-1.0f, 0.0f, 0.0f), vertices, triangles, normals, uvs);
    }

    for (int z = 0; z < MCTileChunk::CHUNK_WIDTH; z++)
    {
        for (int y = 0; y < MCTileChunk::CHUNK_HEIGHT; y++)
        {
            for (int x = 0; x < MCTileChunk::CHUNK_WIDTH; x++)
                planeMap[x][y] = Tile::TEXTURES[tilemap->GetTile(x + offsetX, y, z + offsetZ)][4] == textureID ? (z + offsetZ < MCTilemap::MAP_WIDTH - 1 ? (Tile::TILE_OPAQUE[tilemap->GetTile(x + offsetX, y, z + offsetZ + 1)] ? 0 : 1) : 1) : 0;
        }

        AddPlaneGreedyMesh(planeMap, MCTileChunk::CHUNK_WIDTH, MCTileChunk::CHUNK_HEIGHT, [&vertices, z](int xmin, int ymin, int xmax, int ymax)noexcept
            {
                vertices.emplace_back(glm::vec3(xmax, ymin, z + 1));
                vertices.emplace_back(glm::vec3(xmin, ymin, z + 1));
                vertices.emplace_back(glm::vec3(xmax, ymax, z + 1));
                vertices.emplace_back(glm::vec3(xmin, ymax, z + 1));
            }, glm::vec3(0.0f, 0.0f, 1.0f), vertices, triangles, normals, uvs);
    }

    for (int z = 0; z < MCTileChunk::CHUNK_WIDTH; z++)
    {
        for (int y = 0; y < MCTileChunk::CHUNK_HEIGHT; y++)
        {
            for (int x = 0; x < MCTileChunk::CHUNK_WIDTH; x++)
                planeMap[x][y] = Tile::TEXTURES[tilemap->GetTile(x + offsetX, y, z + offsetZ)][5] == textureID ? (z + offsetZ > 0 ? (Tile::TILE_OPAQUE[tilemap->GetTile(x + offsetX, y, z + offsetZ - 1)] ? 0 : 1) : 1) : 0;
        }

        AddPlaneGreedyMesh(planeMap, MCTileChunk::CHUNK_WIDTH, MCTileChunk::CHUNK_HEIGHT, [&vertices, z](int xmin, int ymin, int xmax, int ymax)noexcept
            {
                vertices.emplace_back(glm::vec3(xmin, ymin, z));
                vertices.emplace_back(glm::vec3(xmax, ymin, z));
                vertices.emplace_back(glm::vec3(xmin, ymax, z));
                vertices.emplace_back(glm::vec3(xmax, ymax, z));
            }, glm::vec3(0.0f, 0.0f, -1.0f), vertices, triangles, normals, uvs);
    }
    vert_cnt = 0;
    idx_cnt = (GLuint)triangles.size();
    return  make_shared<Mesh>(std::move(sVertices), std::move(triangles));
}

void MCTilemapMeshGenerator::AddPlaneGreedyMesh(int map[][MCTileChunk::CHUNK_WIDTH], int mapWidth, int mapHeight, function<void(int, int, int, int)>&& vertexAddCallback, glm::vec3 normal, vector<glm::vec3>& vertices, vector<GLuint>& triangles, vector<glm::vec3>& normals, vector<glm::vec2>& uvs)noexcept
{
    for (int y = 0; y < mapHeight; y++)
    {
        for (int x = 0; x < mapWidth; x++)
        {
            if (map[x][y] != 0)
            {
                int width = 1;

                while (x + width < mapWidth)
                {
                    if (map[x + width][y] == 0)
                        break;
                    width++;
                }

                int height = 1;
                while (y + height < mapHeight)
                {
                    bool condition = true;

                    for (int i = 0; i < width; i++)
                    {
                        if (map[x + i][y + height] == 0)
                        {
                            condition = false;
                            break;
                        }
                    }

                    if (!condition)
                        break;
                    height++;
                }

                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                        map[x + j][y + i] = 0;
                }

                const GLuint triangleIndex = static_cast<GLuint>(vert_cnt) / 4 * 4;
                vertexAddCallback(x, y, x + width, y + height);

                triangles.emplace_back(triangleIndex);
                triangles.emplace_back(triangleIndex + 2);
                triangles.emplace_back(triangleIndex + 1);
                triangles.emplace_back(triangleIndex + 3);
                triangles.emplace_back(triangleIndex + 1);
                triangles.emplace_back(triangleIndex + 2);

                normals.emplace_back(normal);
                normals.emplace_back(normal);
                normals.emplace_back(normal);
                normals.emplace_back(normal);

                uvs.emplace_back(glm::vec2(0.0f, 0.0f));
                uvs.emplace_back(glm::vec2(width, 0.0f));
                uvs.emplace_back(glm::vec2(0.0f, height));
                uvs.emplace_back(glm::vec2(width, height));

                vert_cnt += 4;
                for (ushort index = 0; index < 4; ++index)
                {
                    sVertices.emplace_back(Vertex{
                        vertices[index],
                        normals[index],
                        {},
                         uvs[index],
                          glm::vec4{ ::bitwise_absv(normals[index]),1.f },
                        }
                    );
                }
                vertices.clear();
                normals.clear();
                uvs.clear();
            }
        }
    }
}
