#pragma once
#include "CompFactory.h"
#include "Resource.h"
#include "PathMgr.h"

class Component;
class MonoBehavior;
class Transform;
class MeshRenderer;
class Camera;
class Light;
class InputHandler;
class CoRoutineHandler;
class Animator;
class Collider;
class RigidBody;

class GameObj
	:public enable_shared_from_this<GameObj>
	, public Resource
{
	friend class EventHandler;

	GameObj& SwapGameObj(GameObj& _other)
	{
		SetResName(_other.GetResName());
		SetResPath(_other.GetResPath());
		m_eResType = _other.m_eResType;
		std::swap(m_pParentGameObj, _other.m_pParentGameObj);
		std::swap(m_arrComp, _other.m_arrComp);
		std::swap(m_vecScripts, _other.m_vecScripts);
		std::swap(m_vecChildObj, _other.m_vecChildObj);
		InitGameObj();
		return *this;
	}
protected:
	weak_ptr<GameObj> m_pParentGameObj;
	array<shared_ptr<Component>, COMPONENT_COUNT> m_arrComp;
	vector<shared_ptr<GameObj>> m_vecChildObj;
	vector<shared_ptr<MonoBehavior>> m_vecScripts;
	unordered_map<string, shared_ptr<MonoBehavior>> m_mapScripts;
	//unordered_map<std::type_index, shared_ptr<MonoBehavior>> m_mapScripts;
	bool		m_bIsAlive = true;
	string m_strObjectName;
	//std::atomic<bool> m_bIsUpdateFinish = false;
protected:
	explicit GameObj();
	void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer,const fs::path& _savePath)override;
	void Load(string_view _dirName, const rapidjson::Value& doc,const fs::path& _loadPath) override;
public:
	virtual ~GameObj();
	template <typename T = GameObj> requires std::derived_from<T, GameObj>
	static shared_ptr<T> make_obj();

	GameObj(const GameObj& _other);
	GameObj& operator = (GameObj _other);

	virtual void InitGameObj();
	virtual void Awake();
	virtual void Start();

	virtual void Update();
	virtual void LateUpdate();
	virtual void LastUpdate();
	void MarkTransformDirty()const noexcept;
	void ColliderUpdate()noexcept;
	void FinalUpdate();

	virtual void Render();

	//void ClearComponentWaitFlag()const;

	void AddChild(shared_ptr<GameObj> _pChild);

	shared_ptr<Transform> GetTransform()const;

	void AddComponent(shared_ptr<Component> _pComp);

	template <typename Comp> requires std::derived_from<Comp, Component>
	constexpr const shared_ptr<Comp> AddComponent()
	{
		if constexpr (std::same_as<Comp, Transform>)
			AddComponent(CompFactory::CreateComponent(COMPONENT_TYPE::TRANSFORM));
		else if constexpr (std::same_as<Comp, MeshRenderer>)
			AddComponent(CompFactory::CreateComponent(COMPONENT_TYPE::MESH_RENDERER));
		else if constexpr (std::same_as<Comp, Camera>)
			AddComponent(CompFactory::CreateComponent(COMPONENT_TYPE::CAMERA));
		else if constexpr (std::same_as<Comp, Light>)
			AddComponent(CompFactory::CreateComponent(COMPONENT_TYPE::LIGHT));
		else if constexpr (std::same_as<Comp, InputHandler>)
			AddComponent(CompFactory::CreateComponent(COMPONENT_TYPE::INPUT_HANDLER));
		else if constexpr (std::same_as<Comp, CoRoutineHandler>)
			AddComponent(CompFactory::CreateComponent(COMPONENT_TYPE::COROUTINE_HANDLER));
		else if constexpr ( std::same_as<Comp,Animator> )
			AddComponent(CompFactory::CreateComponent(COMPONENT_TYPE::ANIMATOR));
		else if constexpr (std::same_as<Comp, Collider>)
			AddComponent(CompFactory::CreateComponent(COMPONENT_TYPE::COLLIDER));
		else if constexpr (std::same_as<Comp, RigidBody>)
			AddComponent(CompFactory::CreateComponent(COMPONENT_TYPE::RIGIDBODY));
		else
			assert(false);
		return GetComp<Comp>();
	}

	template <typename Comp> requires std::derived_from<Comp, Component>
	constexpr const shared_ptr<Comp> GetComp()const
	{
		short idx = -1;
		if constexpr (std::same_as<Comp, Transform>)
			idx = etoi(COMPONENT_TYPE::TRANSFORM);
		else if constexpr (std::same_as<Comp, MeshRenderer>)
			idx = etoi(COMPONENT_TYPE::MESH_RENDERER);
		else if constexpr (std::same_as<Comp, Camera>)
			idx = etoi(COMPONENT_TYPE::CAMERA);
		else if constexpr (std::same_as<Comp, Light>)
			idx = etoi(COMPONENT_TYPE::LIGHT);
		else if constexpr (std::same_as<Comp, InputHandler>)
			idx = etoi(COMPONENT_TYPE::INPUT_HANDLER);
		else if constexpr (std::same_as<Comp, CoRoutineHandler>)
			idx = etoi(COMPONENT_TYPE::COROUTINE_HANDLER);
		else if constexpr (std::same_as<Comp, Animator>)
			idx = etoi(COMPONENT_TYPE::ANIMATOR);
		else if constexpr (std::same_as<Comp, Collider>)
			idx = etoi(COMPONENT_TYPE::COLLIDER);
		else if constexpr (std::same_as<Comp, RigidBody>)
			idx = etoi(COMPONENT_TYPE::RIGIDBODY);
		else
			static_assert(std::convertible_to<Comp*,Component*>, "Wrong Component");
		return static_pointer_cast<Comp>(m_arrComp[idx]);
	}

	//void AddScript(shared_ptr<MonoBehavior> _pScript);
	template <typename T> requires std::derived_from<T,MonoBehavior>
	void AddScript(shared_ptr<T> _pScript);
	span<shared_ptr<MonoBehavior>> GetMonoBehaviors() { return m_vecScripts; }
	//const shared_ptr<MonoBehavior>& GetMonoBehavior(string_view _behaviorName)const { return m_mapScripts.find(_behaviorName.data())->second; }
	template <typename T> requires std::derived_from<T, MonoBehavior>
	shared_ptr<T> GetMonoBehavior();
	vector<shared_ptr<GameObj>>& GetChildObj() { return m_vecChildObj; }
	const weak_ptr<GameObj>& GetParentGameObj()const { return m_pParentGameObj; }
	constexpr const bool IsAlive()const noexcept{ return m_bIsAlive; }
	void SetThisObjMainCam()const;
	/*void WaitForObjUpdate() {
		while (!m_bIsUpdateFinish.load()) {
		}
		m_bIsUpdateFinish.store(false);
	}*/
public:
	class iterator
	{
	public:
		using iterator_category = std::input_iterator_tag;
		using value_type = GameObj*;
		using difference_type = std::ptrdiff_t;
		using pointer = GameObj**;
		using reference = GameObj*&;
	private:
		mutable GameObj* m_curObj = nullptr;
		CircleQueue<GameObj*,64> m_bfsQ = {};
	public:
		iterator()noexcept = default;
		~iterator()noexcept = default;
		iterator(GameObj* const _root) noexcept :m_curObj{ _root } { m_bfsQ.emplace(nullptr); }
		const bool operator != (const iterator& other) const noexcept{ return m_curObj != other.m_curObj; }
		const bool operator == (const iterator& other) const noexcept { return m_curObj == other.m_curObj; }
		reference operator * ()const noexcept { return m_curObj; }
		pointer operator -> ()const noexcept { return &m_curObj; }
		iterator& operator ++() noexcept {
			m_bfsQ.pop();
			const auto cache = m_curObj->m_vecChildObj.data();
			const ushort num = (const ushort)m_curObj->m_vecChildObj.size();
			for (ushort i = 0; i < num; ++i)
			{
				m_bfsQ.emplace(cache[i].get());
			}
			m_curObj = m_bfsQ.empty() ? nullptr : m_bfsQ.front();
			return *this;
		}
		iterator operator ++ (int) noexcept {
			iterator temp{ *this };
			++(*this);
			return temp;
		}
	};
	iterator begin() { return iterator{ this }; }
	iterator end() { return iterator{ nullptr }; }
	iterator begin()const { return iterator{ const_cast<GameObj*>(this) }; }
	iterator end()const { return iterator{ nullptr }; }

	void SetResName(string_view _strName) { Resource::SetResName(_strName); }
	const string& GetResName()const { return Resource::GetResName(); }

	void SetObjName(string_view _strName) noexcept { m_strObjectName = _strName; }
	const string& GetObjName()const noexcept { return m_strObjectName; }

	GameObj* FindChildObj(string_view _strName)const {
		const auto iter = std::find_if(this->begin(),this->end(), [_strName](const auto& nodes) {
			return nodes->GetObjName() == _strName;
			});
		return this->end() != iter ? *iter : nullptr;
	}
	GameObj* FindChildObj(const function<bool(GameObj*)>& fp_)const {
		const auto iter = std::find_if(this->begin(), this->end(), fp_);
		return this->end() != iter ? *iter : nullptr;
	}
	auto FindChildObjs(const function<bool(GameObj*)>& fp_)const noexcept {
		return std::views::filter(*this, fp_);
	}
	auto GetAllObj()const noexcept{
		return std::views::all(*this);
	}
	const glm::mat4& GetObjectWorldTransform()const noexcept;
	void Save(string_view _resName,const fs::path& _loadPath = Mgr(PathMgr)->GetSavePath()) override;
	void Load(string_view _dirName,const fs::path& _savePath = Mgr(PathMgr)->GetSavePath()) override;
};

template <typename T> requires std::derived_from<T, GameObj>
shared_ptr<T> GameObj::make_obj()
{
	struct make_shared_enabler :public GameObj {};
	auto pNewObj = std::make_shared<make_shared_enabler>();
	pNewObj->InitGameObj();
	return pNewObj;
}

template<typename T> requires std::derived_from<T, MonoBehavior>
inline void GameObj::AddScript(shared_ptr<T> _pScript)
{
	auto typeName = typeid(T).name();
	auto iter = m_mapScripts.find(typeName);
    if (m_mapScripts.end() != iter)
    {
        return;
    }
    try {
        if (!_pScript->GetGameObjWeak().expired())
            throw std::runtime_error("This Scripts is already exist Owner");
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what();
        exit(0);
    }
    _pScript->SetOwner(weak_from_this());
    m_mapScripts.emplace(std::move(typeName), _pScript);
    m_vecScripts.emplace_back(std::move(_pScript));
}

template<typename T> requires std::derived_from<T, MonoBehavior>
inline shared_ptr<T> GameObj::GetMonoBehavior()
{
	return static_pointer_cast<T>((m_mapScripts.find(typeid(T).name())->second));
}

const bool IsAliveObj(const shared_ptr<GameObj>& obj)noexcept;
const glm::mat4& GetMatrix(const shared_ptr<GameObj>& obj)noexcept;
