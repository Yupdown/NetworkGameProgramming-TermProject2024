#pragma once
#include "Resource.h"

class Texture;

class Material
	:public Resource
{
	friend class ResMgr;
	friend class MeshRenderer;
	friend class Model;
	friend class InstancingMgr;
	friend class ChunkMesh;
private:
	glm::vec3 m_ambient = glm::vec3{ 1.f,1.f,1.f };
	glm::vec3 m_diffuse = glm::vec3{ 1.f,1.f,1.f };
	glm::vec3 m_specular = glm::vec3{ .5f,.5f,.5f };
	float	  m_shininess = 10.f;
	vector<shared_ptr<Texture>> m_vecTex;
private:
	void PushMaterialData();
public:
	Material();
	~Material();

	void AddTexture(shared_ptr<Texture> _pTex) { m_vecTex.emplace_back(_pTex); }
	

	bool AddTexture2D(string_view _strTexName);
	void AddTextureCube(string_view _strTexName);

	void SetMaterialAmbient(const glm::vec3& _ambient) { m_ambient = _ambient; }
	void SetMaterialDiffuse(const glm::vec3& _diffuse) { m_diffuse = _diffuse; }
	void SetMaterialSpecular(const glm::vec3& _specular) { m_specular = _specular; }
	void SetMaterialShine(const float _f) { m_shininess = _f; }

	const glm::vec3& GetMaterialAmbient()const { return m_ambient; }
	const glm::vec3& GetMaterialDiffuse()const { return m_diffuse; }
	const glm::vec3 GetMaterialSpecular()const { return m_specular; }
	constexpr float GetMaterialShine()const { return m_shininess; }


	span<shared_ptr<Texture>> GetTex() { return m_vecTex; }
};

