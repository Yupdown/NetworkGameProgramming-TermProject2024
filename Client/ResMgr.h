#pragma once

#include "Resource.h"
#include "GameObj.h"
#include "Component.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"
#include "Texture2D.h"
#include "CubeMapTex.h"
#include "Model.h"
#include "IterateHashTable.hpp"

struct MeshData
{
	glm::mat4 meshWorldMat = glm::mat4{ 1.f };
	vector<Vertex>* vertexPtr = nullptr;
	vector<GLuint>* indexPtr = nullptr;
};

class ResMgr
	:public Singleton<ResMgr>
{
	friend class Singleton;
	ResMgr();
	~ResMgr();
private:
	using ResourceMap = std::unordered_map<string, shared_ptr<Resource>>;
	using MeshList = IterateHashTable<string, vector<MeshData>>;
	array<ResourceMap, RESOURCE_TYPE_COUNT> m_mapRes;
	std::unordered_map<string, MeshList> m_mapResNameAndMeshData;
private:
	void InitShader();
	void InitTexture2D();
	void InitTextureCube();
	template<typename T> requires std::derived_from<T, Resource>
	RESOURCE_TYPE GetResType()const;
public:
	void Init();
	auto& GetMeshList(string_view strResName)noexcept { return m_mapResNameAndMeshData[strResName.data()]; }
	void AddMeshList(string_view resName,string_view partsName,const glm::mat4& meshMat, vector<Vertex>& v,vector<GLuint>& u)noexcept
	{
		m_mapResNameAndMeshData[resName.data()][partsName.data()].emplace_back(meshMat, &v, &u);
	}
	template<typename T> requires std::derived_from<T,Resource>
	shared_ptr<T> Load(string_view _strFilePath, string_view _strResName);

	template<typename T> requires std::derived_from<T, Resource>
	shared_ptr<T> GetRes(string_view _strResName)const;

	template<typename T> requires std::derived_from<T, Resource>
	bool AddRes(string_view _strResName,shared_ptr<T> _pRes);
};

template<typename T> requires std::derived_from<T, Resource>
inline shared_ptr<T> ResMgr::Load(string_view _strFilePath, string_view _strResName)
{
	const RESOURCE_TYPE resType = GetResType<T>();
	ResourceMap& resMap = m_mapRes[etoi(resType)];

	const auto findIt = resMap.find(_strResName.data());

	if (resMap.end() != findIt)
	{
		return static_pointer_cast<T>(findIt->second);
	}

	const shared_ptr<T> pObj = make_shared<T>();
	pObj->SetResPath(_strFilePath);
	pObj->SetResName(_strResName);
	pObj->Load(_strFilePath, _strResName);
	resMap.emplace_hint(findIt,_strResName,pObj);
	return pObj;
}

template<typename T> requires std::derived_from<T, Resource>
inline shared_ptr<T> ResMgr::GetRes(string_view _strResName)const
{
	const RESOURCE_TYPE resType = GetResType<T>();
	const ResourceMap& resMap = m_mapRes[etoi(resType)];
	
	const auto findIt = resMap.find(_strResName.data());

	if (resMap.end() != findIt)
	{
		return static_pointer_cast<T>(findIt->second);
	}
	else
	{
		try {
			if (resMap.end() == findIt)
				throw std::runtime_error("Please Check Path And FileName\n");
		}
		catch (const std::runtime_error& e) {
			std::cerr << e.what();
			//assert(false);
			//exit(0);
		}
		return nullptr;
	}

}

//template<> 
//inline shared_ptr<Model> ResMgr::GetRes<Model>(string_view _strResName)const
//{
//	const RESOURCE_TYPE resType = GetResType<Model>();
//	const ResourceMap& resMap = m_mapRes[etoi(resType)];
//
//	const auto findIt = resMap.find(_strResName.data());
//
//	if (resMap.end() != findIt)
//	{
//		return std::make_shared<Model>(*static_pointer_cast<Model>(findIt->second));
//	}
//	else
//	{
//		try {
//			if (resMap.end() == findIt)
//				throw std::runtime_error("Please Check Path And FileName\n");
//		}
//		catch (const std::runtime_error& e) {
//			std::cerr << e.what();
//			//assert(false);
//			//exit(0);
//		}
//		return nullptr;
//	}
//}


template<typename T> requires std::derived_from<T, Resource>
inline bool ResMgr::AddRes(string_view _strResName, shared_ptr<T> _pRes)
{
	const RESOURCE_TYPE eType = _pRes->GetResType();
	return m_mapRes[etoi(eType)].try_emplace(_strResName.data(), std::move(_pRes)).second;
}

template<typename T> requires std::derived_from<T, Resource>
inline RESOURCE_TYPE ResMgr::GetResType() const
{
	if constexpr (std::is_same_v<T, GameObj>)
		return RESOURCE_TYPE::GAMEOBJECT;
	else if constexpr (std::is_same_v<T, Material>)
		return RESOURCE_TYPE::MATERIAL;
	else if constexpr (std::is_same_v<T, Mesh>)
		return RESOURCE_TYPE::MESH;
	else if constexpr (std::is_same_v<T, Shader>)
		return RESOURCE_TYPE::SHADER;
	else if constexpr (std::is_same_v<T, Texture2D>)
		return RESOURCE_TYPE::TEXTURE_2D;
	else if constexpr (std::is_same_v<T, CubeMapTex>)
		return RESOURCE_TYPE::TEXTURE_CUBE;
	else if constexpr (std::is_convertible_v<T, Component>)
		return RESOURCE_TYPE::COMPONENT;
	else if constexpr (std::is_same_v<T, Model>)
		return RESOURCE_TYPE::MODEL;
	else
		return  RESOURCE_TYPE::NONE;
}
