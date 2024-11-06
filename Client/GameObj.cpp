#include "pch.h"
#include "GameObj.h"
#include "Component.h"
#include "MonoBehavior.h"
#include "Transform.h"
#include "Scene.h"
#include "SceneMgr.h"
#include "Light.h"
#include "Camera.h"
#include "ThreadMgr.h"
#include "PathMgr.h"
#include "MeshRenderer.h"
#include "InputHandler.h"
#include "CoRoutineHandler.h"
#include "Animator.h"
#include "Core.h"
#include "Collider.h"
#include "Model.h"
#include "RayCaster.h"

using namespace rapidjson;

extern thread_local bool g_bIsSaveFirst;
extern thread_local bool g_bIsLoadFirst;
extern thread_local ofstream outVertexData;
extern thread_local ifstream inVert;

GameObj::GameObj()
    :Resource{RESOURCE_TYPE::GAMEOBJECT}
{
    m_arrComp[etoi(COMPONENT_TYPE::TRANSFORM)] = std::make_shared<Transform>();
    //m_arrComp[etoi(COMPONENT_TYPE::CAMERA)] = std::make_shared<Camera>();
}

GameObj::~GameObj()
{
    // 객체가 삭제될때 트랜스폼의 생명주기를 체크할것
    //if (!m_pParentGameObj.expired())
    //{
    //    const auto pDelTrans = GetTransform();
    //    std::erase_if(pDelTrans->GetParent()->GetChildTransform(),
    //        [&pDelTrans](const weak_ptr<Transform>& p) {return p.lock() == pDelTrans; });
    //}
}

GameObj::GameObj(const GameObj& _other)
    :Resource{ _other }
    , m_strObjectName{ _other.m_strObjectName }
{
    for (size_t idx = 0; idx < _other.m_arrComp.size(); ++idx)
    {
        if (_other.m_arrComp[idx])
        {
            m_arrComp[idx] = _other.m_arrComp[idx]->Comp_Clone();
        }
    }

    for (const auto& othermono : _other.m_vecScripts)
    {
        auto childMono = static_pointer_cast<MonoBehavior>(othermono->Comp_Clone());
        m_mapScripts.emplace(childMono->GetResName(), childMono);
        m_vecScripts.emplace_back(std::move(childMono));
    }

    for (const auto& otherChild : _other.m_vecChildObj)
    {
        auto pChildObj = std::make_shared<GameObj>(*otherChild);
        GetTransform()->AddChild(pChildObj->GetTransform());
        m_vecChildObj.emplace_back(std::move(pChildObj));
    }
}

GameObj& GameObj::operator=(GameObj _other)
{
    return SwapGameObj(_other);
}

void GameObj::Awake()
{
    InitGameObj();

    for (auto& comp : m_arrComp)
    {
        if (comp)
        {
            comp->Awake();
        }
    }
    for (auto& mono : m_vecScripts)
    {
        mono->Awake();
    }
    for (auto& child : m_vecChildObj)
    {
        child->Awake();
    }
}

void GameObj::Start()
{
    for (auto& comp : m_arrComp)
    {
        if (comp)
        {
            comp->Start();
        }
    }
    for (auto& mono : m_vecScripts)
    {
        mono->Start();
    }
    for (auto& child : m_vecChildObj)
    {
        child->Start();
    }
}

void GameObj::Update()
{
    //std::ranges::for_each(m_vecScripts,
    //    std::mem_fn(&MonoBehavior::Update));
    for (const auto& mono : m_vecScripts)
    {
        mono->Update();
    }
   // std::ranges::for_each(m_arrComp
   //     | ::NotComponentNullptr,
   //     std::mem_fn(&Component::Update));
    
    for (const auto& comp : m_arrComp)
    {
        if (comp)
        {
            comp->Update();
        }
    }
   // std::ranges::for_each(m_vecChildObj
   //     | ::OnlyAliveObject
   //     , std::mem_fn(&GameObj::Update));
    for (const auto& child : m_vecChildObj)
    {
        if (child->IsAlive())
        {
            child->Update();
        }
    }

   // m_bIsUpdateFinish.store(true);
}

void GameObj::LateUpdate()
{
    //std::ranges::for_each(m_vecScripts,
    //    std::mem_fn(&MonoBehavior::LateUpdate));
    for (const auto& mono : m_vecScripts)
    {
        mono->LateUpdate();
    }

    //std::ranges::for_each(m_arrComp
    //    | ::NotComponentNullptr,
    //    std::mem_fn(&Component::LateUpdate));

   // for (const auto& comp : m_arrComp)
   // {
   //     if (comp)
   //     {
   //         comp->Update();
   //     }
   // }
   //std::ranges::for_each(m_vecChildObj
   //    | ::OnlyAliveObject
   //    , std::mem_fn(&GameObj::LateUpdate));
    for (const auto& comp : m_arrComp)
    {
        if (comp)
        {
            comp->LateUpdate();
        }
    }
    for (const auto& child : m_vecChildObj)
    {
        if (child->IsAlive())
        {
            child->LateUpdate();
        }
    }
}

void GameObj::LastUpdate()
{
  for (const auto& mono : m_vecScripts)
  {
      mono->LastUpdate();
  }
   // std::ranges::for_each(m_vecScripts,
   //     std::mem_fn(&MonoBehavior::LastUpdate));
   //
   // std::ranges::for_each(m_arrComp
   //     | ::NotComponentNullptr,
   //     std::mem_fn(&Component::LastUpdate));

    //for (const auto& comp : m_arrComp)
    //{
    //    if (comp)
    //    {
    //        comp->Update();
    //    }
    //}
    //std::ranges::for_each(m_vecChildObj
    //    | ::OnlyAliveObject
    //    , std::mem_fn(&GameObj::LastUpdate));
   for (const auto& comp : m_arrComp)
   {
       if (comp)
       {
           comp->LastUpdate();
       }
   }
   for (const auto& child : m_vecChildObj)
   {
       if (child->IsAlive())
       {
           child->LastUpdate();
       }
   }
}
void GameObj::MarkTransformDirty() const noexcept
{
   // std::ranges::for_each(m_arrComp
   //     | ::NotComponentNullptr,
   //     std::mem_fn(&Component::MarkTransformDirty));
  for (const auto& comp : m_arrComp)
  {
      if (comp)
      {
          comp->MarkTransformDirty();
      }
  }
  for (const auto& child : m_vecChildObj)
  {
      if (child->IsAlive())
      {
          child->MarkTransformDirty();
      }
  }
   // std::ranges::for_each(m_vecChildObj
   //     | ::OnlyAliveObject
   //     , std::mem_fn(&GameObj::MarkTransformDirty));
}

void GameObj::ColliderUpdate()noexcept
{
    if (const auto pCollider = static_cast<Collider*>(m_arrComp[etoi(COMPONENT_TYPE::COLLIDER)].get()))
    {
        if (pCollider->IsTransformDirty())
        {
            Mgr(ThreadMgr)->Enqueue(&Collider::UpdateCollider, pCollider);
            //++numOfJob_;
        }
    }
    //std::ranges::for_each(m_vecChildObj
    //    | ::OnlyAliveObject
    //    , [](const shared_ptr<GameObj>& obj)noexcept {
    //        Mgr(ThreadMgr)->Enqueue(&GameObj::ColliderUpdate, obj.get());
    //    });
    const auto cache = m_vecChildObj.data();
    const ushort num = (const ushort)m_vecChildObj.size();
    for (ushort i = 0; i < num; ++i)
    {
        if (cache[i]->IsAlive())
        {
            Mgr(ThreadMgr)->Enqueue(&GameObj::ColliderUpdate, cache[i].get());
        }
    }
}

void GameObj::FinalUpdate()
{
    const auto comp_cache = m_arrComp.data();
    for (unsigned short i = 2; i < COMPONENT_COUNT ; ++i)
    {
        if (comp_cache[i])
        {
            comp_cache[i]->FinalUpdate();
        }
    }
    const auto pTrans = static_cast<Transform*>(comp_cache[0].get());
    pTrans->FinalUpdate();
    if (const auto m = static_cast<MeshRenderer*>(comp_cache[1].get()))
    {
        m->FinalUpdate();
    }
    pTrans->ClearTransformFlag();
   //std::ranges::for_each(m_vecChildObj
   //    | ::OnlyAliveObject
   //    , [](const shared_ptr<GameObj>& obj)noexcept {
   //        Mgr(ThreadMgr)->Enqueue(&GameObj::FinalUpdate, obj.get());
   //    });
   const auto cache = m_vecChildObj.data();
   const ushort num = (const ushort)m_vecChildObj.size();
   for (ushort i = 0; i < num; ++i)
   {
       if (cache[i]->IsAlive())
       {
           Mgr(ThreadMgr)->Enqueue(&GameObj::FinalUpdate, cache[i].get());
       }
   }
}

void GameObj::Render()
{
    //std::ranges::for_each(m_arrComp
    //    | ::NotComponentNullptr,
    //    std::mem_fn(&Component::PreRender));
    for (const auto& comp : m_arrComp)
    {
        if (comp)
        {
            comp->PreRender();
            //Mgr(Core)->AddDrawCall([comp = comp.get()]()noexcept {comp->PreRender(); });
        }
    }
    for (const auto& comp : m_arrComp)
    {
        if (comp)
        {
            comp->Render();
            //Mgr(Core)->AddDrawCall([comp = comp.get()]()noexcept {comp->Render(); });
        }
    }
   //std::ranges::for_each(m_arrComp
   //    | ::NotComponentNullptr,
   //    std::mem_fn(&Component::Render));

    auto& vecChildTransform = GetTransform()->GetChildTransform();
    const auto cache = m_vecChildObj.data();
    for (ushort idx = 0; idx < (const ushort)m_vecChildObj.size();)
    {
        if (cache[idx]->IsAlive())
        {
           // Mgr(ThreadMgr)->Enqueue(&GameObj::ClearComponentWaitFlag, m_vecChildObj[idx].get());
            cache[idx]->Render();
            ++idx;
        }
        else
        {
            idx = RemoveElement(m_vecChildObj, idx);
            idx = RemoveElement(vecChildTransform, idx);
        }
    }
}

//void GameObj::ClearComponentWaitFlag() const
//{
//    for (auto& comp : m_arrComp)
//    {
//        if (comp)
//        {
//            Mgr(ThreadMgr)->Enqueue(&Component::ClearWaitLockAndFlag, comp.get());
//        }
//    }
//    for (auto& mono : m_vecScripts)
//    {
//        Mgr(ThreadMgr)->Enqueue(&Component::ClearWaitLockAndFlag, mono.get());
//    }
//}

void GameObj::AddChild(shared_ptr<GameObj> _pChild)
{
    auto pNewParent = shared_from_this();
    if (pNewParent == _pChild)
    {
        return;
    }
    if (const auto pOldParent = _pChild->m_pParentGameObj.lock())
    {
        if (pOldParent == pNewParent)
        {
            return;
        }
        else
        {
            std::erase(pOldParent->m_vecChildObj, _pChild);
        }
    }
    _pChild->m_pParentGameObj = std::move(pNewParent);
    GetTransform()->AddChild(_pChild->GetTransform());
    _pChild->InitGameObj();
    m_vecChildObj.emplace_back(std::move(_pChild));
}

shared_ptr<Transform> GameObj::GetTransform() const
{
    return static_pointer_cast<Transform>(m_arrComp[etoi(COMPONENT_TYPE::TRANSFORM)]);
}

void GameObj::AddComponent(shared_ptr<Component> _pComp)
{
    _pComp->SetOwner(weak_from_this());

    const int idx = etoi(_pComp->GetCompType());

    if (COMPONENT_TYPE::LIGHT == static_cast<COMPONENT_TYPE>(idx))
    {
        Mgr(SceneMgr)->GetCurScene()->AddLights(static_pointer_cast<Light>(_pComp));
    }
    if (idx < COMPONENT_COUNT)
    {
        m_arrComp[idx] = std::move(_pComp);
    }
    else
    {
        AddScript(static_pointer_cast<MonoBehavior>(std::move(_pComp)));
    }
}

//void GameObj::AddScript(shared_ptr<MonoBehavior> _pScript)
//{
//    auto iter = m_mapScripts.find(_pScript->GetResName());
//    if (m_mapScripts.end() != iter)
//    {
//        return;
//    }
//    try {
//        if (!_pScript->GetGameObjWeak().expired())
//            throw std::runtime_error("This Scripts is already exist Owner");
//    }
//    catch (const std::runtime_error& e) {
//        std::cerr << e.what();
//        exit(0);
//    }
//    _pScript->SetOwner(weak_from_this());
//    m_mapScripts.emplace(_pScript->GetResName(), _pScript);
//    m_vecScripts.emplace_back(std::move(_pScript));
//}

void GameObj::SetThisObjMainCam() const
{
    GetComp<Camera>()->SetMainCam();
}

const glm::mat4& GameObj::GetObjectWorldTransform() const noexcept
{
    return static_cast<const Transform* const>(m_arrComp[etoi(COMPONENT_TYPE::TRANSFORM)].get())->GetLocalToWorldMatrix();
}

void GameObj::Save(string_view _resName,const fs::path& _savePath)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    fs::create_directories(_savePath / _resName);

   
    Save(_resName,writer,_savePath);

    std::ofstream outFile((_savePath / _resName / _resName).string() + ".json");
    outFile << buffer.GetString();

    g_bIsSaveFirst = true;
    outVertexData.close();
}

void GameObj::Load(string_view _dirName,const fs::path& _loadPath)
{
    std::ifstream inFile((_loadPath / _dirName / _dirName).string() + ".json");
    std::string jsonString((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
    rapidjson::Document document;
    document.Parse(jsonString.c_str());

    Load(_dirName, document,_loadPath);

    g_bIsLoadFirst = true;
    inVert.close();
}

void GameObj::Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer,const fs::path& _savePath)
{
    writer.StartObject();

    writer.Key("name");
    writer.String(GetResName().data());

   
    writer.Key("Components");
    writer.StartArray();
    for (auto& comp : m_arrComp)
    {
        writer.StartObject();
        writer.Key("HasComp");
        if (comp)
        {
            writer.Bool(true);
            writer.Key("CompName");
            comp->Save(_resName, writer,_savePath);
        }
        else
        {
            writer.Bool(false);
        }
        writer.EndObject();
    }
    writer.EndArray();
   
    if (!m_vecChildObj.empty())
    {
        writer.Key("Childs");
        writer.StartArray();
        for (auto& child : m_vecChildObj)
        {
            child->Save(_resName, writer,_savePath);
        }
        writer.EndArray();
    }

    writer.EndObject();
}

void GameObj::Load(string_view _dirName, const rapidjson::Value& doc,const fs::path& _loadPath)
{
    //if (doc.HasMember("name") && doc["name"].IsString())
    {
        SetResName(doc["name"].GetString());
    }

   // if (doc.HasMember("Components") && doc["Components"].IsArray()) 
    {
        const rapidjson::Value& componentsArray = doc["Components"];
        
        for (rapidjson::SizeType i = 0; i < componentsArray.Size(); ++i)
        {
           const rapidjson::Value& component = componentsArray[i];
           
           // if (component.HasMember("HasComp"))
            {
                if (component["HasComp"].GetBool())
                {
                    string compName = component["CompName"].GetString();
                    shared_ptr<Component> pComp;
                    if (COMPONENT_TYPE::TRANSFORM == COMPONENT_TYPE(i))
                        pComp = AddComponent<Transform>();
                    else if (COMPONENT_TYPE::MESH_RENDERER == COMPONENT_TYPE(i))
                        pComp = AddComponent<MeshRenderer>();
                    else if (COMPONENT_TYPE::CAMERA == COMPONENT_TYPE(i))
                        pComp = AddComponent<Camera>();
                    else if (COMPONENT_TYPE::LIGHT == COMPONENT_TYPE(i))
                        pComp = AddComponent<Light>();
                    else if (COMPONENT_TYPE::INPUT_HANDLER == COMPONENT_TYPE(i))
                        pComp = AddComponent<InputHandler>();
                    else if (COMPONENT_TYPE::COROUTINE_HANDLER == COMPONENT_TYPE(i))
                        pComp = AddComponent<CoRoutineHandler>();
                    else if (COMPONENT_TYPE::ANIMATOR == COMPONENT_TYPE(i))
                        pComp = AddComponent<Animator>();
                    else if (COMPONENT_TYPE::COLLIDER == COMPONENT_TYPE(i))
                        pComp = AddComponent<Collider>();
                    else
                        assert(false);

                    pComp->Load(_dirName, component,_loadPath);
                    pComp->SetResName(std::move(compName));
                }
            }
        }
    }
    if (doc.HasMember("Childs") && doc["Childs"].IsArray())
    {
        const rapidjson::Value& childs = doc["Childs"];
        for (rapidjson::SizeType i = 0; i < childs.Size(); i++)
        {
            const rapidjson::Value& childValue = childs[i];
            //if (childValue.IsObject())
            {   
                auto pObj = GameObj::make_obj();
                pObj->Load(_dirName, childValue,_loadPath);
                this->AddChild(std::move(pObj));
            }
        }
    }
}

void GameObj::InitGameObj()
{
    m_bIsAlive = true;
    static constexpr const auto view_non_owner = std::views::filter([](const shared_ptr<Component>& _pComp)noexcept {
        return _pComp->GetGameObjWeak().expired(); });
   
    for (const auto& comp : m_arrComp | std::views::filter([](const shared_ptr<Component>& _pComp){
        return static_cast<const bool>(_pComp);}) | view_non_owner)
    {
        comp->SetOwner(weak_from_this());
    }
    for (const auto& mono : m_vecScripts | view_non_owner)
    {
        mono->SetOwner(weak_from_this());
    }
    for (auto& childObj : m_vecChildObj)
    {
        if (childObj->m_pParentGameObj.expired())
        {
            childObj->m_pParentGameObj = weak_from_this();
        }
        childObj->InitGameObj();
    }
}

const bool IsAliveObj(const shared_ptr<GameObj>& obj) noexcept
{
    return obj->IsAlive();
}

const glm::mat4& GetMatrix(const shared_ptr<GameObj>& obj) noexcept
{
    return obj->GetObjectWorldTransform();
}
