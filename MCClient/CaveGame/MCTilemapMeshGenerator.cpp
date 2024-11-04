#include "pch.h"

#include "MCTilemapMeshGenerator.h"
#include "MCTilemap.h"

#include <Vertex.hpp>
#include <Mesh.h>
#include <ResMgr.h>
#include "MeshRenderer.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "GameObj.h"
#include "Material.h"
#include "Transform.h"
#include "EventMgr.h"
#include "ChunkMesh.h"
#include "ChunkRendererObject.h"
#include "Player.h"

extern std::atomic_bool g_bTileFinish;

static glm::vec4 GetAOLevel(shared_ptr<MCTilemap> tilemap, const glm::ivec3& side1, const glm::ivec3& side2, const glm::ivec3& corner)
{
    bool bSide1 = Tile::TILE_OPAQUE[tilemap->GetTile(side1)];
    bool bSide2 = Tile::TILE_OPAQUE[tilemap->GetTile(side2)];
    bool bCorner = Tile::TILE_OPAQUE[tilemap->GetTile(corner)];
    int level = 0;
    if (!bSide1 || !bSide2)
        level = 3 - (bSide1 + bSide2 + bCorner);
    return glm::vec4(glm::vec3(0.75f - level * 0.25f), 0.0f);
}

void MCTilemapMeshGenerator::CreateMeshAll(shared_ptr<MCTilemap> tilemap, array<array<shared_ptr<ChunkRendererObject>, MCTilemap::CHUNK_SIZE>, MCTilemap::CHUNK_SIZE>& pOut) const noexcept
{
    //g_bTileFinish.store(false);

    //shared_ptr<Material> material;
    //shared_ptr<ChunkMesh> pChunkData;

    //material = make_shared<Material>();
    //material->AddTexture2D("mctile.png");
    //pChunkData = make_shared<ChunkMesh>(tilemap);
    //pChunkData->SetChunkMaterial(material);
    //tilemap->AddNotifyCallback([pChunkData](MCTileChunk* pChunk, int chunkX, int chunkZ) noexcept { pChunkData->OnChunkMeshChanged(pChunk, chunkX, chunkZ); });

    //vector<std::future<void>> generate_mesh;
    //generate_mesh.reserve(MCTilemap::CHUNK_SIZE * MCTilemap::CHUNK_SIZE);
    //for (int chunkX = 0; chunkX < MCTilemap::CHUNK_SIZE; ++chunkX)
    //{
    //    for (int chunkZ = 0; chunkZ < MCTilemap::CHUNK_SIZE; ++chunkZ)
    //    {
    //        shared_ptr<GameObj> terrainObj = GameObj::make_obj();
    //        auto renderer = terrainObj->AddComponent<MeshRenderer>();
    //        generate_mesh.emplace_back(Mgr(ThreadMgr)->EnqueueTaskFuture([this, tilemap, chunkX, chunkZ, renderer]()noexcept {
    //            shared_ptr<Mesh> mesh = this->CreateMeshFromChunk(tilemap, chunkX, chunkZ);
    //            renderer->AddMesh(mesh);
    //            }));
    //        MCTileChunk* pChunk = tilemap->GetChunk(chunkX, chunkZ);
    //        terrainObj->GetTransform()->SetLocalPosition({ chunkX * MCTileChunk::CHUNK_WIDTH, 0.0f, chunkZ * MCTileChunk::CHUNK_WIDTH });

    //        pChunkData->AddChunk(terrainObj, pChunk);
    //        pChunkData->SetChunkMeshTexID(1);
    //    }
    //}
    //for (auto& f : generate_mesh)f.get();
    //Mgr(EventMgr)->AddEventNeedLock([pChunkData]() noexcept
    //    {
    //        const auto curScene = Mgr(SceneMgr)->GetCurScene();
    //        Mgr(ThreadMgr)->Enqueue(&ChunkMesh::MergeMeshData, pChunkData.get());
    //        Mgr(ThreadMgr)->WaitAllJob();
    //        pChunkData->InitChunkMesh("DefaultWarpShader.glsl");
    //        curScene->AddChunkMesh(0, pChunkData);
    //        static_pointer_cast<Player>(curScene->GetPlayer())->InitCamDirection();
    //        g_bTileFinish.store(true);
    //    });

    g_bTileFinish.store(false);

    for (int chunkX = 0; chunkX < MCTilemap::CHUNK_SIZE; ++chunkX)
    {
        for (int chunkZ = 0; chunkZ < MCTilemap::CHUNK_SIZE; ++chunkZ)
        {
            shared_ptr<Mesh> mesh = this->CreateMeshFromChunk(tilemap, chunkX, chunkZ);
            pOut[chunkX][chunkZ]->ChangeMeshData(mesh->GetVertices(), mesh->GetIndicies());
            MCTileChunk* pChunk = tilemap->GetChunk(chunkX, chunkZ);
            pOut[chunkX][chunkZ]->GetTransform()->SetLocalPosition({ chunkX * MCTileChunk::CHUNK_WIDTH, 0.0f, chunkZ * MCTileChunk::CHUNK_WIDTH });
        }
    }
    g_bTileFinish.store(true);
}

shared_ptr<Mesh> MCTilemapMeshGenerator::CreateMeshFromChunk(const shared_ptr<MCTilemap>& tilemap, int chunkX, int chunkZ) noexcept
{
    const MCTileChunk* const chunk = tilemap->GetChunk(chunkX, chunkZ);
    const int offsetX = chunkX * MCTileChunk::CHUNK_WIDTH;
    const int offsetZ = chunkZ * MCTileChunk::CHUNK_WIDTH;

    vector<glm::vec3> vertices; vertices.reserve(13040);
    vector<glm::vec3> normals; normals.reserve(13040);
    vector<glm::vec2> uvs; uvs.reserve(13040);
    vector<glm::vec4> colors; colors.reserve(13040);
    vector<GLuint> triangles; triangles.reserve(19560);
   
    for (int x = 0; x < MCTileChunk::CHUNK_WIDTH; ++x)
    {
        for (int y = 0; y < MCTileChunk::CHUNK_HEIGHT; ++y)
        {
            for (int z = 0; z < MCTileChunk::CHUNK_WIDTH; ++z)
            {
                if (chunk->GetTile(x, y, z) == 0)
                    continue;
                for (int d = 0; d < 6; ++d)
                {
                    const int dx = "110211"[d] - '1';
                    const int dy = "201111"[d] - '1';
                    const int dz = "111120"[d] - '1';
                    const int xp = offsetX + x + dx;
                    const int yp = y + dy;
                    const int zp = offsetZ + z + dz;

                    bool bounds = xp < 0 || yp < 0 || zp < 0 || xp >= MCTilemap::MAP_WIDTH || yp >= MCTilemap::MAP_HEIGHT || zp >= MCTilemap::MAP_WIDTH;
                    if (!bounds && Tile::TILE_OPAQUE[tilemap->GetTile(xp, yp, zp)])
                        continue;

                    switch (d)
                    {
                    // Upward quad
                    case 0:
                        vertices.emplace_back(glm::vec3(x, y + 1, z));
                        vertices.emplace_back(glm::vec3(x + 1, y + 1, z));
                        vertices.emplace_back(glm::vec3(x, y + 1, z + 1));
                        vertices.emplace_back(glm::vec3(x + 1, y + 1, z + 1));
                        colors.emplace_back(GetAOLevel(tilemap, { xp - 1, yp, zp }, { xp, yp, zp - 1 }, { xp - 1, yp, zp - 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp + 1, yp, zp }, { xp, yp, zp - 1 }, { xp + 1, yp, zp - 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp - 1, yp, zp }, { xp, yp, zp + 1 }, { xp - 1, yp, zp + 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp + 1, yp, zp }, { xp, yp, zp + 1 }, { xp + 1, yp, zp + 1 }));
                        break;
                    // Downward quad
                    case 1:
						vertices.emplace_back(glm::vec3(x + 1, y, z));
						vertices.emplace_back(glm::vec3(x, y, z));
						vertices.emplace_back(glm::vec3(x + 1, y, z + 1));
						vertices.emplace_back(glm::vec3(x, y, z + 1));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp, zp - 1 }, { xp + 1, yp, zp }, { xp + 1, yp, zp - 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp, zp - 1 }, { xp - 1, yp, zp }, { xp - 1, yp, zp - 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp, zp + 1 }, { xp + 1, yp, zp }, { xp + 1, yp, zp + 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp, zp + 1 }, { xp - 1, yp, zp }, { xp - 1, yp, zp + 1 }));
						break;
                    // Leftward quad
                    case 2:
                        vertices.emplace_back(glm::vec3(x, y, z + 1));
						vertices.emplace_back(glm::vec3(x, y, z));
						vertices.emplace_back(glm::vec3(x, y + 1, z + 1));
						vertices.emplace_back(glm::vec3(x, y + 1, z));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp - 1, zp }, { xp, yp, zp + 1 }, { xp, yp - 1, zp + 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp - 1, zp }, { xp, yp, zp - 1 }, { xp, yp - 1, zp - 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp + 1, zp }, { xp, yp, zp + 1 }, { xp, yp + 1, zp + 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp + 1, zp }, { xp, yp, zp - 1 }, { xp, yp + 1, zp - 1 }));
						break;
                    // Rightward quad
                    case 3:
                        vertices.emplace_back(glm::vec3(x + 1, y, z));
						vertices.emplace_back(glm::vec3(x + 1, y, z + 1));
                        vertices.emplace_back(glm::vec3(x + 1, y + 1, z));
						vertices.emplace_back(glm::vec3(x + 1, y + 1, z + 1));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp - 1, zp }, { xp, yp, zp - 1 }, { xp, yp - 1, zp - 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp - 1, zp }, { xp, yp, zp + 1 }, { xp, yp - 1, zp + 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp + 1, zp }, { xp, yp, zp - 1 }, { xp, yp + 1, zp - 1 }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp, yp + 1, zp }, { xp, yp, zp + 1 }, { xp, yp + 1, zp + 1 }));
                        break;
                    // Forward quad
                    case 4:
						vertices.emplace_back(glm::vec3(x + 1, y, z + 1));
                        vertices.emplace_back(glm::vec3(x, y, z + 1));
                        vertices.emplace_back(glm::vec3(x + 1, y + 1, z + 1));
                        vertices.emplace_back(glm::vec3(x, y + 1, z + 1));
                        colors.emplace_back(GetAOLevel(tilemap, { xp + 1, yp, zp }, { xp, yp - 1, zp }, { xp + 1, yp - 1, zp }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp - 1, yp, zp }, { xp, yp - 1, zp }, { xp - 1, yp - 1, zp }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp + 1, yp, zp }, { xp, yp + 1, zp }, { xp + 1, yp + 1, zp }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp - 1, yp, zp }, { xp, yp + 1, zp }, { xp - 1, yp + 1, zp }));
						break;
                    // Backward quad
                    case 5:
                        vertices.emplace_back(glm::vec3(x, y, z));
						vertices.emplace_back(glm::vec3(x + 1, y, z));
						vertices.emplace_back(glm::vec3(x, y + 1, z));
						vertices.emplace_back(glm::vec3(x + 1, y + 1, z));
                        colors.emplace_back(GetAOLevel(tilemap, { xp - 1, yp, zp }, { xp, yp - 1, zp }, { xp - 1, yp - 1, zp }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp + 1, yp, zp }, { xp, yp - 1, zp }, { xp + 1, yp - 1, zp }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp - 1, yp, zp }, { xp, yp + 1, zp }, { xp - 1, yp + 1, zp }));
                        colors.emplace_back(GetAOLevel(tilemap, { xp + 1, yp, zp }, { xp, yp + 1, zp }, { xp + 1, yp + 1, zp }));
						break;
                    }

                    unsigned int triangleIndex = static_cast<unsigned int>(vertices.size() - 4);

                    float uvUnitX = 1.0f / 12.0f;
                    float uvOffsetX = static_cast<float>(Tile::TEXTURES[chunk->GetTile(x, y, z)][d] - 1) * uvUnitX;

                    float a00 = (colors.end() - 4)->r;
                    float a01 = (colors.end() - 3)->r;
                    float a10 = (colors.end() - 2)->r;
                    float a11 = (colors.end() - 1)->r;

                    if (a00 + a11 > a01 + a10)
                    {
                        triangles.emplace_back(triangleIndex);
                        triangles.emplace_back(triangleIndex + 2);
                        triangles.emplace_back(triangleIndex + 1);
                        triangles.emplace_back(triangleIndex + 3);
                        triangles.emplace_back(triangleIndex + 1);
                        triangles.emplace_back(triangleIndex + 2);
                    }
                    else
                    {
                        triangles.emplace_back(triangleIndex);
                        triangles.emplace_back(triangleIndex + 2);
                        triangles.emplace_back(triangleIndex + 3);
                        triangles.emplace_back(triangleIndex + 3);
                        triangles.emplace_back(triangleIndex + 1);
                        triangles.emplace_back(triangleIndex + 0);
                    }

                    const glm::vec3 normal = glm::vec3(dx, dy, dz);
                    normals.emplace_back(normal);
                    normals.emplace_back(normal);
                    normals.emplace_back(normal);
                    normals.emplace_back(normal);

                    uvs.emplace_back(glm::vec2(uvOffsetX, 0.0f));
                    uvs.emplace_back(glm::vec2(uvOffsetX + uvUnitX, 0.0f));
                    uvs.emplace_back(glm::vec2(uvOffsetX, 1.0f));
                    uvs.emplace_back(glm::vec2(uvOffsetX + uvUnitX, 1.0f));
                }
            }
        }
    }

    vector<Vertex> sVertices;
    sVertices.reserve(vertices.size());
    for (uint index = 0; index < vertices.size(); ++index)
        sVertices.emplace_back(vertices[index], normals[index], glm::zero<glm::vec3>(), uvs[index], colors[index]);
    return make_shared<Mesh>(std::move(sVertices), std::move(triangles));
}
