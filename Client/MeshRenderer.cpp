#include "pch.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "Shader.h"
#include "Transform.h"
#include "SceneMgr.h"
#include "GameObj.h"
#include "Material.h"
#include "Texture.h"
#include "ResMgr.h"
#include "PathMgr.h"
#include "Model.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "ThreadMgr.h"
#include "Camera.h"

MeshRenderer::MeshRenderer()
	:Component{COMPONENT_TYPE::MESH_RENDERER}
{
	//m_pMaterial = make_shared<Material>();
}

MeshRenderer::~MeshRenderer()
{
}

MeshRenderer::MeshRenderer(const MeshRenderer& other)
	:Component{COMPONENT_TYPE::MESH_RENDERER}
	,m_vecMesh{other.m_vecMesh}
	,m_vecMaterial{other.m_vecMaterial}
	,m_pShader{other.m_pShader}
	,m_mapModelData{other.m_mapModelData}
{
	if (other.m_pModelData)
	{
		m_pModelData = std::make_shared<Model>(*other.m_pModelData);
	}
}

void MeshRenderer::DeleteModelData(string_view _strModelName)
{
	m_mapModelData.erase(_strModelName.data());
	m_pModelData->DeleteModel(_strModelName);
}

void MeshRenderer::DeleteModelDataAndChild(string_view _strModelName)
{
	m_mapModelData.erase(_strModelName.data());
	const auto iter = std::find_if(m_pModelData->begin(), m_pModelData->end(), [_strModelName](const Model* const m) {
		return m->GetResName() == _strModelName; });
	if (m_pModelData->end() == iter)
	{
		return;
	}
	if (const auto pParent = (*iter)->GetParentModel())
	{
		std::erase_if(pParent->GetChildModel(), [iter](const shared_ptr<Model>& p) { return p.get() == (*iter); });
	}
	else
	{
		m_pModelData.reset();
	}
}

void MeshRenderer::SetShader(string_view _strShaderName)
{
	m_pShader = Mgr(ResMgr)->GetRes<Shader>(_strShaderName);
}

void MeshRenderer::SetModelData(shared_ptr<Model> pModelData_) noexcept
{
	if (m_pModelData)
	{
		m_mapModelData.clear();
	}
	m_pModelData = std::move(pModelData_);
	if (m_mapModelData.empty())
	{
		m_pModelData->InitModel();
		for (const auto& model : *m_pModelData)
		{
			m_mapModelData.emplace(model->GetResName(), model->shared_from_this());
		}
	}
}

void MeshRenderer::FinalUpdate()
{
	if (!m_pModelData || m_bIsIntancing)
	{
		return;
	}
	const auto pTrans = GetTransform();
	//if (m_bTransformDirty || pTrans->IsDirty())
	//{
	//	m_pModelData->GetModelTransform()->SetMatrix(pTrans->GetLocalToWorldMatrix() * m_pModelData->m_matWorld);
	//}
	m_pModelData->UpdateModelTransform();
}

void MeshRenderer::PreRender() const
{
	if (m_bIsIntancing || !m_bIsActivate)
	{
		return;
	}
	m_pShader->Use();
	//Mgr(SceneMgr)->GetCurScene()->PreRender(m_pShader);

	if (!m_pModelData)
	{
		m_pShader->SetUniformMat4(GetTransform()->GetLocalToWorldMatrix(), "uModel");

		for (const auto& material : m_vecMaterial)
		{
			material->PushMaterialData();
			for (const auto& tex : material->GetTex())
			{
				tex->BindTexture();
			}
		}
	}
	else
	{
		for (const auto& material : m_vecMaterial)
		{
			material->PushMaterialData();
			for (const auto& tex : material->GetTex())
			{
				tex->BindTexture();
			}
		}
	}
}

void MeshRenderer::Render() const
{
	if (m_bIsIntancing || !m_bIsActivate)
	{
		return;
	}
	if (m_pModelData)
	{
		m_pModelData->Render(m_pShader,m_vecMaterial.empty(),GetTransform()->GetLocalToWorldMatrix());
	}
	else
	{
		for (const auto& mesh : m_vecMesh)
		{
			mesh->Render();
		}
		for (const auto& material : m_vecMaterial)
		{
			for (const auto& tex : material->GetTex())
			{
				tex->UnBindTexture();
			}
		}
	}
}

void MeshRenderer::Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer,const fs::path& _savePath)
{
	writer.String("MeshRenderer");

	writer.Key("ShaderName");
	writer.String(m_pShader->GetResName().data());

	writer.Key("Model");
	writer.StartObject();  
	m_pModelData->Save(_resName, writer ,_savePath);  
	writer.EndObject();
}

void MeshRenderer::Load(string_view _dirName, const rapidjson::Value& doc,const fs::path& _loadPath)
{
	SetShader(doc["ShaderName"].GetString());
	if (doc.HasMember("Model"))
	{
		m_pModelData = make_shared<Model>();
		m_pModelData->Load(_dirName, doc["Model"], _loadPath);
		Mgr(ResMgr)->AddRes(m_pModelData->GetResName(), m_pModelData);
	}
	for (const auto& models : *m_pModelData)
	{
		m_mapModelData.emplace(models->GetResName(), models->shared_from_this());
	}
}

void MeshRenderer::SetMaterialAmbient(const glm::vec3& _ambient)
{
	//m_vecMaterial.back()->SetMaterialAmbient(_ambient);
}

void MeshRenderer::SetMaterialDiffuse(const glm::vec3& _diffuse)
{
	//m_vecMaterial.back()->SetMaterialDiffuse(_diffuse);
}

void MeshRenderer::SetMaterialSpecular(const glm::vec3& _specular)
{
	//m_vecMaterial.back()->SetMaterialSpecular(_specular);
}

void MeshRenderer::SetMaterialShine(const float _f)
{
	//m_vecMaterial.back()->SetMaterialShine(_f);
}
