#include "pch.h"
#include "Model.h"
#include "Material.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "ThreadMgr.h"
#include "Transform.h"

thread_local constinit bool g_bIsSaveFirst = true;
thread_local constinit bool g_bIsLoadFirst = true;
thread_local ofstream outVertexData;
thread_local ifstream inVert;

Model::Model()
	:Resource{ RESOURCE_TYPE::MODEL }
	,m_pModelTransform{ make_shared<Transform>() }
{
}

Model::~Model()
{
	if (const auto pParent = m_pParentModel.lock())
	{
		std::erase_if(pParent->m_pModelTransform->GetChildTransform()
			, [trans =m_pModelTransform.get()](const weak_ptr<Transform>& p) {return p.lock().get() == trans; });
	}
}

Model::Model(const Model& other)
	:Resource{RESOURCE_TYPE::MODEL}
	,m_vecMesh{other.m_vecMesh}
	,m_vecMaterial{other.m_vecMaterial}
	,m_bIsActivate{other.m_bIsActivate}
	,m_pModelTransform{static_pointer_cast<Transform>(other.m_pModelTransform->Comp_Clone())}
{
	for (const auto& otherChild : other.m_vecChildModel)
	{
		auto childModel = std::make_shared<Model>(*otherChild);
		childModel->SetResName(otherChild->GetResName());
		m_pModelTransform->AddChild(childModel->m_pModelTransform);
		m_vecChildModel.emplace_back(std::move(childModel));
	}
}

void Model::SetModelMat(const glm::mat4& mat_)
{
	m_pModelTransform->SetMatrix(mat_);
}

void Model::AddChild(shared_ptr<Model> childModel_)
{
	childModel_->m_pParentModel = weak_from_this();
	m_pModelTransform->AddChild(childModel_->m_pModelTransform);
	m_vecChildModel.emplace_back(std::move(childModel_));
}

void Model::InitModel() noexcept
{
	for (auto& childs : m_vecChildModel)
	{
		childs->m_pParentModel = weak_from_this();
		childs->InitModel();
	}
}

void Model::UpdateModelTransform() noexcept
{
	m_pModelTransform->UpdateTransfromHierarchy();
}

void Model::Render(const shared_ptr<Shader>& pShader_, const bool bIsMeshRendererMaterialEmpty_, const glm::mat4& ownerMatrix)noexcept
{
	if (m_bIsActivate)
	{
		pShader_->SetUniformMat4(ownerMatrix * m_pModelTransform->GetLocalToWorldMatrix(), "uModel");

		for (auto& material : m_vecMaterial)
		{
			if (bIsMeshRendererMaterialEmpty_)
			{
				material->PushMaterialData();
			}
			for (const auto& tex : material->GetTex())
			{
				tex->BindTexture();
			}
		}

		for (auto& mesh : m_vecMesh)
		{
			mesh->Render();
		}

		for (auto& material : m_vecMaterial)
		{
			for (const auto& tex : material->GetTex())
			{
				tex->UnBindTexture();
			}
		}
	}

	for (auto& child : m_vecChildModel)
	{
		child->Render(pShader_, bIsMeshRendererMaterialEmpty_,ownerMatrix);
	}
}

void Model::Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath)
{
	//writer.StartObject();

	writer.Key("ModelName");
	writer.String(GetResName().data());

	

	writer.Key("Mesh");

	if (g_bIsSaveFirst)
	{
		outVertexData.open((_savePath / _resName / _resName).string() + ".vertex", std::ios::binary);
		g_bIsSaveFirst = false;
		if (!outVertexData)
		{
			assert(false);
		}
	}

	writer.Int((int)m_vecMesh.size());
	size_t num;
	for (auto& mesh : m_vecMesh)
	{
		num = mesh->m_vecVertex.size();
		outVertexData.write((char*)&num, sizeof(size_t));
		outVertexData.write((char*)mesh->m_vecVertex.data(), mesh->m_vecVertex.size() * sizeof(Vertex));
		num = mesh->m_vecIdx.size();
		outVertexData.write((char*)&num, sizeof(size_t));
		outVertexData.write((char*)mesh->m_vecIdx.data(), mesh->m_vecIdx.size() * sizeof(GLuint));
	}

	writer.Key("Material");
	writer.StartArray();
	for (auto& material : m_vecMaterial)
	{
		writer.StartObject();

		writer.Key("Ambient");
		writer.StartArray();
		writer.Double(material->m_ambient.r);
		writer.Double(material->m_ambient.g);
		writer.Double(material->m_ambient.b);
		writer.EndArray();

		writer.Key("Diffuse");
		writer.StartArray();
		writer.Double(material->m_diffuse.r);
		writer.Double(material->m_diffuse.g);
		writer.Double(material->m_diffuse.b);
		writer.EndArray();

		writer.Key("Specular");
		writer.StartArray();
		writer.Double(material->m_specular.r);
		writer.Double(material->m_specular.g);
		writer.Double(material->m_specular.b);
		writer.EndArray();

		writer.Key("Shininess");
		writer.Double(material->m_shininess);

		writer.Key("Texture");
		writer.StartArray();
		for (auto& tex : material->m_vecTex)
		{
			writer.StartObject();

			writer.Key("TextureName");
			writer.String(tex->GetResName().data());

			writer.EndObject();
		}
		writer.EndArray();

		writer.EndObject();
	}
	writer.EndArray();

	writer.Key("Transform"); 
	writer.StartArray();

	const glm::mat4 tempMat = m_pModelTransform->GetLocalMatrix();

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			writer.Double(tempMat[i][j]);
		}
	}
	writer.EndArray();

	if (!m_vecChildModel.empty())
	{
		writer.Key("ChildModel");
		writer.StartArray();
		for (const auto& child : m_vecChildModel)
		{
			writer.StartObject();
			child->Save(_resName, writer, _savePath);
			writer.EndObject();
		}
		writer.EndArray();
	}

	//writer.EndObject();
}

void Model::Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath)
{
	if (g_bIsLoadFirst)
	{
		inVert.open((_loadPath / _dirName / _dirName).string() + ".vertex", std::ios::binary);
		g_bIsLoadFirst = false;
		if (!inVert)
		{
			assert(false);
		}
	}
	
	SetResName(doc["ModelName"].GetString());

	size_t numOfMesh = {};
	size_t numOfVert = {};
	size_t numOfIdx = {};
	//if (doc.HasMember("Mesh"))
	{
		numOfMesh = (size_t)doc["Mesh"].GetInt();
	}

	m_vecMesh.reserve(numOfMesh);
	m_vecMaterial.reserve(numOfMesh);

	for (int i = 0; i < numOfMesh; ++i)
	{
		inVert.read((char*)&numOfVert, sizeof(size_t));
		vector<Vertex> v(numOfVert);
		inVert.read((char*)v.data(), sizeof(Vertex) * numOfVert);
		inVert.read((char*)&numOfIdx, sizeof(size_t));
		vector<GLuint> idx(numOfIdx);
		inVert.read((char*)idx.data(), sizeof(GLuint) * numOfIdx);
		auto pMesh = make_shared<Mesh>();
		pMesh->InitForLoad(std::move(v), std::move(idx));
		m_vecMesh.emplace_back(std::move(pMesh));
	}

	const rapidjson::Value& materialArray = doc["Material"];

	//if (materialArray.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < materialArray.Size(); ++i)
		{
			const rapidjson::Value& materialValue = materialArray[i];

			auto pMaterial = make_shared<Material>();

			//if (true)
			{
				const rapidjson::Value& ambientValue = materialValue["Ambient"];
				//if (ambientValue.IsArray() && ambientValue.Size() == 3)
				{
					const float ambientR = (float)ambientValue[0].GetDouble();
					const float ambientG = (float)ambientValue[1].GetDouble();
					const float ambientB = (float)ambientValue[2].GetDouble();
					const glm::vec3 ambient(ambientR, ambientG, ambientB);
					pMaterial->SetMaterialAmbient(ambient);
				}

				const rapidjson::Value& diffuseValue = materialValue["Diffuse"];
				//if (diffuseValue.IsArray() && diffuseValue.Size() == 3)
				{
					const float diffuseR = (float)diffuseValue[0].GetDouble();
					const float diffuseG = (float)diffuseValue[1].GetDouble();
					const float diffuseB = (float)diffuseValue[2].GetDouble();
					const glm::vec3 diffuse(diffuseR, diffuseG, diffuseB);
					pMaterial->SetMaterialDiffuse(diffuse);
				}

				const rapidjson::Value& specularValue = materialValue["Specular"];
				//if (specularValue.IsArray() && specularValue.Size() == 3)
				{
					const float specularR = (float)specularValue[0].GetDouble();
					const float specularG = (float)specularValue[1].GetDouble();
					const float specularB = (float)specularValue[2].GetDouble();
					const glm::vec3 specular(specularR, specularG, specularB);
					pMaterial->SetMaterialSpecular(specular);
				}

				const float shininess = (float)materialValue["Shininess"].GetDouble();

				pMaterial->SetMaterialShine(shininess);

				const rapidjson::Value& texturesValue = materialValue["Texture"];
				//if (texturesValue.IsArray())
				{
					for (rapidjson::SizeType j = 0; j < texturesValue.Size(); ++j)
					{
						const rapidjson::Value& textureValue = texturesValue[j];
						//if (textureValue.IsObject() && textureValue.HasMember("TextureName"))
						{
							const char* textureName = textureValue["TextureName"].GetString();
							pMaterial->AddTexture2D(textureName);
						}
					}
				}
			}
			m_vecMaterial.emplace_back(std::move(pMaterial));
		}
	}

	const rapidjson::Value& matrixArray = doc["Transform"];

	if (matrixArray.IsArray() && matrixArray.Size() == 16) 
	{
		glm::mat4 tempMat;

		int index = 0;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				tempMat[i][j] = static_cast<float>(matrixArray[index++].GetDouble());
			}
		}
		m_pModelTransform->SetMatrix(tempMat);
	}

	if (doc.HasMember("ChildModel") && doc["ChildModel"].IsArray())
	{
		const rapidjson::Value& childs = doc["ChildModel"];
		for (rapidjson::SizeType i = 0; i < childs.Size(); i++)
		{
			const rapidjson::Value& childValue = childs[i];
			{
				auto pChild = make_shared<Model>();
				pChild->Load(_dirName, childValue, _loadPath);
				this->AddChild(std::move(pChild));
			}
		}
	}
}

bool Model::DeleteModel(string_view strModelName_)
{
	const auto iter = std::ranges::find_if(m_vecChildModel, [strModelName_](const auto& a) {return a->GetResName() == strModelName_; });
	bool bFind = false;
	if (m_vecChildModel.end() != iter)
	{
		auto delChilds = std::move((*iter)->m_vecChildModel);
		this->m_vecChildModel.erase(iter);
		for (auto& childs : delChilds)
		{
			this->AddChild(std::move(childs));
		}
		return true;
	}
	else
	{
		for (auto& child : m_vecChildModel)
		{
			bFind = child->DeleteModel(strModelName_);
			if (bFind)
			{
				break;
			}
		}
	}
	return bFind;
}
