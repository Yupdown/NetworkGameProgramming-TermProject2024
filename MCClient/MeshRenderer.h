#pragma once
#include "Component.h"

class Mesh;
class Material;
class Shader;
class Model;

class MeshRenderer :
    public Component
{
    friend class InstancingMgr;
public:
    COMP_CLONE(MeshRenderer)
private:
    vector<shared_ptr<Mesh>> m_vecMesh;
    vector<shared_ptr<Material>> m_vecMaterial;
  //  shared_ptr<Material> m_pMaterial;
    shared_ptr<Model> m_pModelData;
    shared_ptr<Shader> m_pShader;
    unordered_map<string, shared_ptr<Model>> m_mapModelData;
    bool m_bIsIntancing = false;
    bool m_bIsActivate = true;
public:
    MeshRenderer();
    ~MeshRenderer();
    MeshRenderer(const MeshRenderer& other);
public:
    auto& GetModelMap()noexcept { return m_mapModelData; }
    const shared_ptr<Model>& GetModel(string_view _strModelName)const { return m_mapModelData.find(_strModelName.data())->second; }
    const shared_ptr<Model>& GetModel()const { return m_pModelData; }
    void DeleteModelData(string_view _strModelName);
    void DeleteModelDataAndChild(string_view _strModelName);
    void SetShader(string_view _strShaderName);
    const shared_ptr<Shader>& GetShader()const { return m_pShader; }

    void AddMesh(shared_ptr<Mesh> _pMesh) { m_vecMesh.emplace_back(std::move(_pMesh)); }
    std::span<shared_ptr<Mesh>> GetMesh()noexcept { return m_vecMesh; }

   // void SetMaterial(shared_ptr<Material> _pMaterial) { m_pMaterial = std::move(_pMaterial); }
   // shared_ptr<Material> GetMaterial() const { return m_pMaterial; }

    void AddMaterial(shared_ptr<Material> _pMaterial) { m_vecMaterial.emplace_back(std::move(_pMaterial)); }
    std::span<shared_ptr<Material>> GetMaterial() { return m_vecMaterial; }

    void SetModelData(shared_ptr<Model> pModelData_)noexcept;
   // void AddTexture2D(string_view _strTexName);
   // void AddTextureCube(string_view _strTexName);
  
    void FinalUpdate()override;
    void PreRender()const override;
    void Render()const override;

    virtual void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer,const fs::path& _savePath) override;
    virtual void Load(string_view _dirName, const rapidjson::Value& doc,const fs::path& _loadPath) override;
public:
    void SetMaterialAmbient(const glm::vec3& _ambient);
    void SetMaterialDiffuse(const glm::vec3& _diffuse);
    void SetMaterialSpecular(const glm::vec3& _specular);
    void SetMaterialShine(const float _f);

    void SetIsActivate(const bool bFlag)noexcept { m_bIsActivate = bFlag; }
};

