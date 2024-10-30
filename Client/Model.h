#pragma once
#include "Resource.h"

class Mesh;
class Material;
struct BoneInfo;
struct Animation;
class Shader;
class Transform;

class Model
	:public Resource
	,public enable_shared_from_this<Model>
{
	friend class AssimpMgr;
	friend class InstancingMgr;
	friend class MeshRenderer;
private:
	weak_ptr<Model> m_pParentModel;
	vector<shared_ptr<Model>> m_vecChildModel;
	vector<shared_ptr<Mesh>> m_vecMesh;
	vector<shared_ptr<Material>> m_vecMaterial;
	shared_ptr<Transform> m_pModelTransform;
	glm::mat4 m_matLocal = glm::mat4{ 1.f };
	glm::mat4 m_matWorld = glm::mat4{ 1.f };
	bool m_bIsActivate = true;
public:
	Model();
	~Model();
	Model(const Model& other);
	shared_ptr<Model> GetParentModel()const { return m_pParentModel.lock(); }
	std::span<shared_ptr<Material>> GetModelMaterial() { return m_vecMaterial; }
	void AddMesh(shared_ptr<Mesh> _pMesh) { m_vecMesh.emplace_back(std::move(_pMesh)); }
	void AddMaterial(shared_ptr<Material> _pMaterial) { m_vecMaterial.emplace_back(std::move(_pMaterial)); }
	void SetModelMat(const glm::mat4& mat_);
	void AddChild(shared_ptr<Model> childModel_);
	void InitModel()noexcept;
	void UpdateModelTransform()noexcept;

	void Render(const shared_ptr<Shader>& pShader_, const bool bIsMeshRendererMaterialEmpty_,const glm::mat4& ownerMatrix=glm::mat4{1.f})noexcept;
public:
	bool IsActivate()const { return m_bIsActivate; }
	void SetActivate(const bool b_) { m_bIsActivate = b_; }
public:
	virtual void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath) override;
	virtual void Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath) override;
	
public:
	class iterator
	{
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = Model*;
		using difference_type = std::ptrdiff_t;
		using pointer = Model**;
		using reference = Model*&;
	private:
		mutable Model* m_curObj = nullptr;
		std::queue<Model*> m_bfsQ = {};
	public:
		iterator() = default;
		~iterator() = default;
		iterator(Model* _root) :m_curObj{ _root } { m_bfsQ.emplace(nullptr); }
		const bool operator != (const iterator& other) const { return m_curObj != other.m_curObj; }
		const bool operator == (const iterator& other) const { return m_curObj == other.m_curObj; }
		reference operator * ()const { return m_curObj; }
		pointer operator -> ()const { return &m_curObj; }
		iterator& operator ++() {
			m_bfsQ.pop();
			for (auto& child : m_curObj->m_vecChildModel)
				m_bfsQ.emplace(child.get());
			m_curObj = m_bfsQ.empty() ? nullptr : std::move(m_bfsQ.front());
			return *this;
		}
		iterator operator ++ (int) {
			iterator temp{ *this };
			++(*this);
			return temp;
		}
	};
	iterator begin() { return iterator{ this }; }
	iterator end() { return iterator{ nullptr }; }
	iterator begin()const { return iterator{ const_cast<Model*>(this) }; }
	iterator end()const { return iterator{ nullptr }; }

	const bool needInit()const noexcept {
		return !m_vecChildModel.empty() && m_vecChildModel.back()->m_pParentModel.expired();
	}

	const shared_ptr<Transform>& GetModelTransform()const noexcept { return m_pModelTransform; }

	Model* FindModel(string_view strModelName_) {
		const auto iter = std::find_if(this->begin(),this->end(), [strModelName_](const Model* a) {return a->GetResName() == strModelName_; });
		return this->end() != iter ? *iter : nullptr;
	}
	auto FindModels(const function<bool(Model*)>& fp_)const noexcept {
		return std::views::filter(*this, fp_);
	}
	auto GetAllModel()const noexcept {
		return std::views::all(*this);
	}
	auto GetAllModelValidViews()const noexcept {
		return GetAllModel() | std::views::filter([](const Model* const m)noexcept {return !m->m_vecMesh.empty(); });
	}
	vector<Model*> GetAllValidModel()const noexcept {
		vector<Model*> temp;
		for (auto& child : GetAllModelValidViews())
		{
			temp.emplace_back(child);
		}
		return temp;
	}
	bool DeleteModel(string_view strModelName_);
	vector<shared_ptr<Model>>& GetChildModel() { return m_vecChildModel; }
};

