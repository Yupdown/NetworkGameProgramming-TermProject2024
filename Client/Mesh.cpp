#include "pch.h"
#include "Mesh.h"
#include "ThreadMgr.h"

Mesh::Mesh()
    :Resource{RESOURCE_TYPE::MESH}
{
}

Mesh::~Mesh()
{
   //glDeleteBuffers(1, &vbo);
   //glDeleteBuffers(1, &ebo);
   //glDeleteVertexArrays(1, &vao);
}

void Mesh::InitForLoad(vector<Vertex>&& _vertices, vector<GLuint>&& _indices)
{
    m_vecVertex = std::move(_vertices);
    m_vecIdx = std::move(_indices);
    Mgr(ThreadMgr)->EnqueueTaskForMainThread(&Mesh::SetBuffers, this);
}

void Mesh::SetBuffers()
{
    m_numOfVertices = (GLuint)m_vecVertex.size();
    m_numOfIndices = (GLuint)m_vecIdx.size();
    m_vecVertex.shrink_to_fit();
    m_vecIdx.shrink_to_fit();

    std::atomic_thread_fence(std::memory_order_seq_cst);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vecVertex.size() * sizeof(Vertex), m_vecVertex.data(), GL_STATIC_DRAW);

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

    /*glVertexAttribPointer(5, 4, GL_INT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneIds));
    glEnableVertexAttribArray(5);

    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, boneWeights));
    glEnableVertexAttribArray(6);*/

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vecIdx.size() * sizeof(GLuint), m_vecIdx.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    // TODO
    // CPU데이터들 지우기 추가했음
    m_numOfVertices = (GLuint)m_vecVertex.size();
    m_numOfIndices = (GLuint)m_vecIdx.size();
   // m_vecVertex.clear();
   // m_vecIdx.clear();
}

void Mesh::Render() const
{
    glBindVertexArray(vao);
    glDrawElements(m_ePolygonMode, (GLsizei)m_numOfIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

