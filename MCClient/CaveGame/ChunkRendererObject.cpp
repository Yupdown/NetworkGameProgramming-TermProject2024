#include "pch.h"
#include "ChunkRendererObject.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Texture.h"
#include "ResMgr.h"
#include "Transform.h"
#include "MCTilemapMeshGenerator.h"
#include "MCTilemap.h"
#include "ThreadMgr.h"

ChunkRendererObject::ChunkRendererObject()
{
	BindBuffers();
}

ChunkRendererObject::~ChunkRendererObject()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void ChunkRendererObject::Initialize(shared_ptr<Shader> pShader, shared_ptr<Material> pMaterial)
{
	m_pShader = pShader;
	m_pMaterial = pMaterial;
}

void ChunkRendererObject::BindBuffers()
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_numOfVertices * sizeof(Vertex), m_vecChunkVertex.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(4);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numOfIndices * sizeof(GLsizei), m_vecChunkIndex.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void ChunkRendererObject::RebindBuffers()
{
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_numOfVertices * sizeof(Vertex), m_vecChunkVertex.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numOfIndices * sizeof(GLsizei), m_vecChunkIndex.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
}

void ChunkRendererObject::Render()
{
    m_pShader->Use();
    glBindVertexArray(vao);
    m_pMaterial->PushMaterialData();

    for (const auto& tex : m_pMaterial->GetTex())
    {
        tex->BindTexture();
    }
    m_pShader->SetUniformMat4(GetObjectWorldTransform(), "uModel");
    glDrawElements(GL_TRIANGLES, m_numOfIndices, GL_UNSIGNED_INT, 0);
    for (const auto& tex : m_pMaterial->GetTex())
    {
        tex->UnBindTexture();
    }
    glBindVertexArray(0);
}

void ChunkRendererObject::ChangeMeshData(const vector<Vertex>& vertices, const vector<unsigned int>& indices)
{
    m_vecChunkVertex = vertices;
	m_vecChunkIndex = indices;
	m_numOfVertices = static_cast<GLuint>(vertices.size());
	m_numOfIndices = static_cast<GLuint>(indices.size());

	RebindBuffers();
}