#pragma once
#include "GameObj.h"

class Material;
class Shader;
class MCTileChunk;
class MCTilemap;

struct ChunkInfo
{
	shared_ptr<Mesh> refMesh = nullptr;
	glm::mat4 worldMat = glm::mat4{ 1.f };
};

class ChunkMesh
	:public GameObj
{
private:
	std::mutex m_mt[2];
	SpinLock m_spinLock;
	vector<std::future<void>> m_vecFutureForReConstruct;
	
	vector<size_t> m_vecVertexSize;
	vector<ChunkInfo> m_vecChunkInfo;
	shared_ptr<MCTilemap> m_pTileMapForReDrawMesh = nullptr;
	GLuint m_iChunkTexID = 0;

	vector<Vertex> m_vecChunkVertex;
	vector<GLsizei> m_vecChunkIndex;

	std::vector<GLsizei> m_indexCounts;
	vector<const void*> m_indexOffsets;


	GLuint vao, vbo, ebo;
	GLuint m_numOfVertices = 0;
	GLuint m_numOfIndices = 0;
	shared_ptr<Material> m_pChunckMeshMaterial;
	shared_ptr<Shader> m_pChunckMeshShader;
	
	unordered_map<MCTileChunk*, GLuint> m_mapChunkToIndex;
	bool m_bDirty = false;
private:
	void ReConstructVertex(vector<Vertex>& v1, vector<Vertex>& v2, shared_ptr<Mesh> mesh, const GLuint idx)noexcept;
	void ReConstructIndex(vector<GLuint>& i1)noexcept;
	void ReBindMesh()noexcept;
public:
	ChunkMesh(const shared_ptr<MCTilemap>& pTileMap);
	~ChunkMesh();

	void MergeMeshData()noexcept;

	void SetChunkMaterial(shared_ptr<Material> pMaterial)noexcept { m_pChunckMeshMaterial = pMaterial; }

	void InitChunkMesh(string_view strShaderName)noexcept;

	void Render()override;

	void OnChunkMeshChanged(MCTileChunk* const pChunk, int chunkX, int chunkZ)noexcept;

	void SetChunkMeshTexID(const GLuint iTexID)noexcept { m_iChunkTexID = iTexID; }

	void AddChunk(shared_ptr<GameObj> pChild, MCTileChunk* pChunk)noexcept;
};

