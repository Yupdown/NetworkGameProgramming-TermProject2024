#pragma once

#include "pch.h"
#include "GameObj.h"

class Mesh;
class Material;
class Shader;
class MCTileChunk;
class MCTilemap;

class ChunkRendererObject : public GameObj
{
public:
	ChunkRendererObject();
	~ChunkRendererObject();
	void Initialize(shared_ptr<Shader> pShader, shared_ptr<Material> pMaterial);

	void BindBuffers();
	void RebindBuffers();

	void Render() override;
	void ChangeMeshData(const vector<Vertex>& vertices, const vector<unsigned int>& indices);

private:
	vector<Vertex> m_vecChunkVertex;
	vector<unsigned int> m_vecChunkIndex;

	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint m_numOfVertices = 0;
	GLuint m_numOfIndices = 0;

	shared_ptr<Shader> m_pShader;
	shared_ptr<Material> m_pMaterial;
};