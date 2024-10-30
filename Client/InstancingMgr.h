#pragma once
#include "pch.h"
#include "IterateHashTable.hpp"

class Model;
class GameObj;
class Shader;
class Mesh;
class Transform;
class Material;
class Texture;

struct RenderInfo
{
	GLuint meshVAO;
	GLuint numOfIndices;
	shared_ptr<Material> pMaterialData;
};

class InstancingMgr
	:public Singleton<InstancingMgr>
{
	friend class Singleton;
	InstancingMgr();
	~InstancingMgr();
private:
	//using InstancingList = std::unordered_map<string, vector<shared_ptr<GameObj>>>;
	//using InstancingPartsAndModelMat = std::map<string,glm::mat4>;
	//using InstancingPartsAndUpdateMat = std::map<string, vector<glm::mat4>>;
	//using InstancingPartsAndRenderInfo = std::map<string, vector<RenderInfo>>;

	using InstancingList = IterateHashTable<string, vector<shared_ptr<GameObj>>>;
	using InstancingPartsAndModelMat = IterateHashTable<string, glm::mat4>;

	using InstancingPartsAndUpdateMat = std::unordered_map<string, vector<glm::mat4>>;

	using InstancingPartsAndRenderInfo = IterateHashTable<string, vector<RenderInfo>>;

	InstancingList m_InstanceList;
	std::unordered_map<string, InstancingPartsAndModelMat> m_mapResNameAndPartsModelMat;
	std::unordered_map<string, InstancingPartsAndUpdateMat> m_mapResNameAndPartsUpdateMat;
	std::unordered_map<string, InstancingPartsAndRenderInfo> m_mapResNameAndPartsRenderInfo;
	std::unordered_set<string> m_setResNameForUnique;

	std::unordered_map<string,shared_ptr<Texture>> m_mapEnvMappingObj;

	using TempBlock = std::aligned_storage<sizeof(std::shared_ptr<GameObj>), alignof(std::shared_ptr<GameObj>)>::type;
	//vector<vector<shared_ptr<GameObj>>> m_vecTemporaryObject;
	//vector<vector<TempBlock>> m_vecTemporaryObject;
	shared_ptr<Shader> m_pShaderForInstancing;
	shared_ptr<Shader> m_pShaderForInstancingAndEnv;
	GLuint instanceVBO;
private:
	void InitInstanceData(string_view strResName,shared_ptr<Model> pModel,const bool bIsEnv_ = false)noexcept;
public:
	void Init();
	void AddInstancingList(shared_ptr<GameObj> pObj)noexcept;
	void SetAllObjMaterials(string_view strResName,shared_ptr<Material> pMaterial)noexcept;
	void SetObjPartsMaterials(string_view strResName,string_view strPartsName, shared_ptr<Material> pMaterial)noexcept;

	void SetObjMaterialSpecular(string_view strResName,const glm::vec3 spec)noexcept;
	void SetObjMaterialDiffuse(string_view strResName,const glm::vec3 diffuse)noexcept;
	void SetObjMaterialAmbient(string_view strResName,const glm::vec3 ambient)noexcept;
	void SetObjMaterialShine(string_view strResName,const float shine)noexcept;


	void SetObjPartsMaterialSpecular(string_view strResName, string_view strPartsName,const glm::vec3 spec)noexcept;
	void SetObjPartsMaterialDiffuse(string_view strResName, string_view strPartsName, const glm::vec3 diffuse)noexcept;
	void SetObjPartsMaterialAmbient(string_view strResName, string_view strPartsName, const glm::vec3 ambient)noexcept;
	void SetObjPartsMaterialShine(string_view strResName, string_view strPartsName, const float shine)noexcept;

	void Update() noexcept;
	void Render() noexcept;
	void Reset()noexcept
	{
		//m_vecTemporaryObject.clear();
		m_mapEnvMappingObj.clear();
		m_InstanceList.clear();
		m_mapResNameAndPartsModelMat.clear();
		m_mapResNameAndPartsUpdateMat.clear();
		m_mapResNameAndPartsRenderInfo.clear();
		m_setResNameForUnique.clear();
	}
};

