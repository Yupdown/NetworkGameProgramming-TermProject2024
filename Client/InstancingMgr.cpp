#include "pch.h"
#include "InstancingMgr.h"
#include "GameObj.h"
#include "MeshRenderer.h"
#include "Model.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture2D.h"
#include "Transform.h"
#include "Shader.h"
#include "ResMgr.h"
#include "Core.h"
#include "ThreadMgr.h"
#include "EventMgr.h"

static constexpr inline void _FastMemcpy(unsigned char* pDes, unsigned char* pSrc, unsigned long nCopySize)noexcept
{
	unsigned long nLoopCount = nCopySize;

	--pDes; --pSrc;
	nLoopCount >>= 3;

	switch (nCopySize & 7)
	{
	case 0:
		do {
			*(++pDes) = *(++pSrc);
	case 7: *(++pDes) = *(++pSrc);
	case 6: *(++pDes) = *(++pSrc);
	case 5: *(++pDes) = *(++pSrc);
	case 4: *(++pDes) = *(++pSrc);
	case 3: *(++pDes) = *(++pSrc);
	case 2: *(++pDes) = *(++pSrc);
	case 1: *(++pDes) = *(++pSrc);
		} while (--nLoopCount);
	}
}

static constexpr inline void _FastMemcpy16(unsigned char* pDes, unsigned char* pSrc, unsigned long nCopySize)noexcept
{
	unsigned long nLoopCount = nCopySize;

	--pDes; --pSrc;
	nLoopCount >>= 4;

	switch (nCopySize & 15) 
	{
	case 0: do {
		*(++pDes) = *(++pSrc);
	case 15: *(++pDes) = *(++pSrc);
	case 14: *(++pDes) = *(++pSrc);
	case 13: *(++pDes) = *(++pSrc);
	case 12: *(++pDes) = *(++pSrc);
	case 11: *(++pDes) = *(++pSrc);
	case 10: *(++pDes) = *(++pSrc);
	case 9: *(++pDes) = *(++pSrc);
	case 8: *(++pDes) = *(++pSrc);
	case 7: *(++pDes) = *(++pSrc);
	case 6: *(++pDes) = *(++pSrc);
	case 5: *(++pDes) = *(++pSrc);
	case 4: *(++pDes) = *(++pSrc);
	case 3: *(++pDes) = *(++pSrc);
	case 2: *(++pDes) = *(++pSrc);
	case 1: *(++pDes) = *(++pSrc);
	} while (--nLoopCount);
	}
}

InstancingMgr::InstancingMgr()
{
	
}

InstancingMgr::~InstancingMgr()
{
	glDeleteBuffers(1, &instanceVBO);
}

void InstancingMgr::InitInstanceData(string_view strResName,shared_ptr<Model> pModel, const bool bIsEnv_) noexcept
{
	const shared_ptr<Shader>& target_shader = bIsEnv_ ? m_pShaderForInstancingAndEnv : m_pShaderForInstancing;

	target_shader->Use();

	const ushort num = (const ushort)pModel->m_vecMesh.size();
	for (ushort i=0;i< num ;++i)
	{
		const GLuint meshVAO = pModel->m_vecMesh[i]->vao;
		glBindVertexArray(meshVAO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		for (ushort j = 0; j < 4; ++j)
		{
			glEnableVertexAttribArray(5 + j);
			glVertexAttribPointer(5 + j, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * j));
			glVertexAttribDivisor(5 + j, 1);
		}
		auto& partsRenderInfo = m_mapResNameAndPartsRenderInfo[strResName.data()][pModel->GetResName()];
		partsRenderInfo.emplace_back(RenderInfo{ meshVAO,pModel->m_vecMesh[i]->m_numOfIndices,pModel->m_vecMaterial[i] });
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void InstancingMgr::Init()
{
	m_pShaderForInstancing = Mgr(ResMgr)->GetRes<Shader>("InstancingShader.glsl");
	m_pShaderForInstancingAndEnv = Mgr(ResMgr)->GetRes<Shader>("EnvironmentShaderInstancing.glsl");

	m_InstanceList.reserve(1000);
	m_mapResNameAndPartsModelMat.reserve(1000);
	m_mapResNameAndPartsUpdateMat.reserve(1000);
	m_mapResNameAndPartsRenderInfo.reserve(1000);
	m_setResNameForUnique.reserve(1000);
	m_mapEnvMappingObj.reserve(1000);

	m_pShaderForInstancing->Use();

	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, 15000 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
}

void InstancingMgr::AddInstancingList(shared_ptr<GameObj> pObj) noexcept
{
	const auto mr = pObj->GetComp<MeshRenderer>();
	const auto& modelMap = mr->GetModelMap();
	
	mr->m_bIsIntancing = true;
	const string strResName = pObj->GetResName();

	NAGOX_ASSERT(!strResName.empty(), "Instancing Object must need Resource Name");

	if (not m_setResNameForUnique.contains(strResName))
	{
		string_view shaderName = mr->GetShader()->GetResName();

		const bool bIsEnv = shaderName == "EnvironmentShaderInstancing.glsl" || shaderName == "EnvironmentShader.glsl";

		for (const auto& [modelName, modelData] : modelMap)
		{
			//modelData->m_bIsActivate = false;
			if (not modelData->m_vecMesh.empty())
			{
				m_mapResNameAndPartsModelMat[strResName][modelName] = modelData->m_matWorld;
				InitInstanceData(strResName,modelData,bIsEnv);
			}
		}
		m_InstanceList[strResName].emplace_back(pObj);
		m_setResNameForUnique.emplace(strResName);

		if (bIsEnv)
		{
			m_mapEnvMappingObj.emplace(strResName,mr->GetMaterial().front()->GetTex().front());
		}
	}
	else
	{
		m_InstanceList[strResName].emplace_back(pObj);
	}
}

void InstancingMgr::SetAllObjMaterials(string_view strResName, shared_ptr<Material> pMaterial) noexcept
{
	NAGOX_ASSERT(m_InstanceList.contains(strResName.data()), "There is no Instancing Object");

	for (auto& [partsName, renderInfoList] : m_mapResNameAndPartsRenderInfo[strResName.data()])
	{
		for (auto& renderInfo : renderInfoList)
		{
			renderInfo.pMaterialData = pMaterial;
		}
	}
}

void InstancingMgr::SetObjPartsMaterials(string_view strObjName, string_view strPartsName, shared_ptr<Material> pMaterial) noexcept
{
	for (auto& renderInfo : m_mapResNameAndPartsRenderInfo[strObjName.data()][strPartsName.data()])
	{
		renderInfo.pMaterialData = pMaterial;
	}
}

void InstancingMgr::SetObjMaterialSpecular(string_view strObjName, const glm::vec3 spec) noexcept
{
	NAGOX_ASSERT(m_InstanceList.contains(strObjName.data()), "There is no Instancing Object");

	for (const auto& [partsName, renderInfoList] : m_mapResNameAndPartsRenderInfo[strObjName.data()])
	{
		for (const auto& renderInfo : renderInfoList)
		{
			renderInfo.pMaterialData->SetMaterialSpecular(spec);
		}
	}
}

void InstancingMgr::SetObjMaterialDiffuse(string_view strObjName, const glm::vec3 diffuse) noexcept
{
	NAGOX_ASSERT(m_InstanceList.contains(strObjName.data()), "There is no Instancing Object");

	for (const auto& [partsName, renderInfoList] : m_mapResNameAndPartsRenderInfo[strObjName.data()])
	{
		for (const auto& renderInfo : renderInfoList)
		{
			renderInfo.pMaterialData->SetMaterialDiffuse(diffuse);
		}
	}
}

void InstancingMgr::SetObjMaterialAmbient(string_view strObjName, const glm::vec3 ambient) noexcept
{
	NAGOX_ASSERT(m_InstanceList.contains(strObjName.data()), "There is no Instancing Object");

	for (const auto& [partsName, renderInfoList] : m_mapResNameAndPartsRenderInfo[strObjName.data()])
	{
		for (const auto& renderInfo : renderInfoList)
		{
			renderInfo.pMaterialData->SetMaterialAmbient(ambient);
		}
	}
}

void InstancingMgr::SetObjMaterialShine(string_view strObjName, const float shine) noexcept
{
	NAGOX_ASSERT(m_InstanceList.contains(strObjName.data()), "There is no Instancing Object");

	for (const auto& [partsName, renderInfoList] : m_mapResNameAndPartsRenderInfo[strObjName.data()])
	{
		for (const auto& renderInfo : renderInfoList)
		{
			renderInfo.pMaterialData->SetMaterialShine(shine);
		}
	}
}

void InstancingMgr::SetObjPartsMaterialSpecular(string_view strObjName, string_view strPartsName, const glm::vec3 spec) noexcept
{
	for (const auto& renderInfo : m_mapResNameAndPartsRenderInfo[strObjName.data()][strPartsName.data()])
	{
		renderInfo.pMaterialData->SetMaterialSpecular(spec);
	}
}

void InstancingMgr::SetObjPartsMaterialDiffuse(string_view strObjName, string_view strPartsName, const glm::vec3 diffuse) noexcept
{
	for (const auto& renderInfo : m_mapResNameAndPartsRenderInfo[strObjName.data()][strPartsName.data()])
	{
		renderInfo.pMaterialData->SetMaterialDiffuse(diffuse);
	}
}

void InstancingMgr::SetObjPartsMaterialAmbient(string_view strObjName, string_view strPartsName, const glm::vec3 ambient) noexcept
{
	for (const auto& renderInfo : m_mapResNameAndPartsRenderInfo[strObjName.data()][strPartsName.data()])
	{
		renderInfo.pMaterialData->SetMaterialAmbient(ambient);
	}
}

void InstancingMgr::SetObjPartsMaterialShine(string_view strObjName, string_view strPartsName, const float shine) noexcept
{
	for (const auto& renderInfo : m_mapResNameAndPartsRenderInfo[strObjName.data()][strPartsName.data()])
	{
		renderInfo.pMaterialData->SetMaterialShine(shine);
	}
}

void InstancingMgr::Update() noexcept
{
	for (auto& [resName, objList] : m_InstanceList)
	{ 
		const GLuint num = (const GLuint)objList.size();
		auto tempVec = std::make_shared_for_overwrite<TempBlock[]>(num);
		::memcpy(tempVec.get(), objList.data(), sizeof(shared_ptr<GameObj>) * num);
		Mgr(ThreadMgr)->Enqueue([this,tempVec = std::move(tempVec), &resName, num]()noexcept {
			for (const auto& [partsName, modelMat] : m_mapResNameAndPartsModelMat[resName])
			{
				auto& updateMatrixVec = m_mapResNameAndPartsUpdateMat[resName][partsName];
				updateMatrixVec.clear();
				Mgr(ThreadMgr)->Enqueue([&updateMatrixVec, tempVec, &modelMat, num]()noexcept {
					const auto obj_block = tempVec.get();
					for (GLuint i = 0; i < num; ++i)
					{
						if (const shared_ptr<const GameObj>& obj = reinterpret_cast<const shared_ptr<const GameObj>&>(obj_block[i]);
							obj->IsAlive())
						{
							updateMatrixVec.emplace_back(obj->GetObjectWorldTransform() * modelMat);
						}
					}
					});
			}
			});
		Mgr(ThreadMgr)->Enqueue([&objList]()noexcept {
			std::erase_if(objList,
			[](const shared_ptr<GameObj>& p)noexcept {return !p->IsAlive(); });
			});
	}
}

void InstancingMgr::Render() noexcept
{
	Mgr(ThreadMgr)->WaitAllJob();

	Mgr(EventMgr)->CheckMemPool();

	for (const auto& resName : m_InstanceList | std::views::keys)
	{
		const auto iter = m_mapEnvMappingObj.find(resName);
		if (m_mapEnvMappingObj.end() != iter)
		{
			m_pShaderForInstancingAndEnv->Use();
			iter->second->BindTexture();
		}
		else
		{
			m_pShaderForInstancing->Use();
		}
		
		for (const auto& [partsName, renderInfoList] : m_mapResNameAndPartsRenderInfo[resName])
		{
			for (const auto& renderInfo : renderInfoList)
			{
				glBindVertexArray(renderInfo.meshVAO);
				const auto& material = renderInfo.pMaterialData;
				material->PushMaterialData();
				for (const auto& tex : material->GetTex())
				{
					tex->BindTexture();
				}
				const auto& matVec = m_mapResNameAndPartsUpdateMat[resName][partsName];
				glBindBuffer(GL_ARRAY_BUFFER, this->instanceVBO);
				glBufferSubData(GL_ARRAY_BUFFER, 0, (GLuint)matVec.size() * sizeof(glm::mat4), matVec.data());
				glDrawElementsInstanced(GL_TRIANGLES, renderInfo.numOfIndices,
					GL_UNSIGNED_INT, 0, (GLuint)matVec.size());
				for (const auto& tex : material->GetTex())
				{
					tex->UnBindTexture();
				}
				glBindBuffer(GL_ARRAY_BUFFER,0);
				glBindVertexArray(0);
			}
		}
	}
}
