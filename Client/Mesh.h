#pragma once
#include "Resource.h"

class Shader;

class Mesh
	:public Resource
{
	friend class MeshRenderer;
	friend class AssimpMgr;
	friend class Model;
	friend class InstancingMgr;
private:
	vector<Vertex> m_vecVertex;
	vector<GLuint> m_vecIdx;
	GLuint vao, vbo, ebo;
	GLuint m_numOfVertices = 0;
	GLuint m_numOfIndices = 0;
	GLenum m_ePolygonMode = GL_TRIANGLES;
public:
	Mesh();
	~Mesh();
	vector<Vertex>& GetVertices() noexcept { return m_vecVertex; }
	vector<GLuint>& GetIndicies() noexcept { return m_vecIdx; }

	const vector<Vertex>& GetVertices() const noexcept { return m_vecVertex; }
	const vector<GLuint>& GetIndicies() const noexcept { return m_vecIdx; }

	template <typename V, typename I>
	Mesh(V&& _vertices, I&& _indices) : Resource{RESOURCE_TYPE::MESH}
		,m_vecVertex{ std::forward<V>(_vertices) }, m_vecIdx{ std::forward<I>(_indices) } { }

	template <typename V, typename I>void Init(V&& _vertices, I&& _indices)
	{
		m_vecVertex = std::move(_vertices);
		m_vecIdx = std::move(_indices);
		SetBuffers();
	}

	void InitForLoad(vector<Vertex>&& _vertices, vector<GLuint>&& _indices);
	
	template <typename vertex> requires std::same_as <vertex,Vertex>
	void AddVertex(vertex&& _vertex) { m_vecVertex.emplace_back(std::forward<vertex>(_vertex)); }

	template <typename vertetices> requires std::convertible_to <vertetices, std::vector<Vertex>>
	void AddVertex(vertetices&& _vertetices) { m_vecVertex = std::forward<vertetices>(std::move(_vertetices)); }

	template <typename... Args>void AddIdx(Args&&... args){(m_vecIdx.emplace_back(args), ...);}

	void AddIdx(const vector<array<GLuint, 3>>& _indicies) { for (auto& arr : _indicies)m_vecIdx.insert(m_vecIdx.end(), arr.begin(), arr.end()); }

	template <typename indices> requires std::convertible_to <indices, std::vector<GLuint>>
	void AddIdx(indices&& _indices) { m_vecIdx = std::forward<indices>(std::move(_indices)); }

	virtual void Render()const;

	void SetBuffers();

	void SetPolygonMode(GLenum mode) noexcept { m_ePolygonMode = mode; }
};

