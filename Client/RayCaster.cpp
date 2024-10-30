#include "pch.h"
#include "RayCaster.h"
#include "KeyMgr.h"
#include "Camera.h"
#include "Core.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Collider.h"
#include "GameObj.h"
#include "Transform.h"
#include "ThreadMgr.h"
#include "ResMgr.h"

static constexpr const auto cmp = [](const GameObj* const a, const GameObj* const b)noexcept {
    const auto viewMat = Camera::GetMainCamViewMat();
    return (viewMat * glm::vec4{ a->GetTransform()->GetWorldPosition(),1.f }).z < (viewMat * glm::vec4{ b->GetTransform()->GetWorldPosition(),1.f }).z;
    };

std::set<GameObj*, decltype(cmp)> setPickingObject{ cmp };
std::map<GameObj*,glm::vec3, decltype(cmp)> setPickingObjectMesh{ cmp };

RayCaster::RayCaster()
{
}

RayCaster::~RayCaster()
{
}

void RayCaster::Init()
{
    m_mapMeshRayIntersectTable.reserve(1024);
}

void RayCaster::RegisterOBBPickingList(weak_ptr<GameObj> pCol_) noexcept
{
    const auto pCollider = pCol_.lock()->GetComp<Collider>();
    NAGOX_ASSERT(pCollider, "OBB Picking Needs Collider");
    m_vecPickingObjList.emplace_back(std::move(pCol_));
    pCollider->SetIsPicking(true);
}

void RayCaster::RegisterMeshRayIntersect(const shared_ptr<GameObj>& pObj) noexcept
{
    m_mapMeshRayIntersectTable.emplace(pObj, std::make_pair(false, glm::vec3{}));
   
    if (const auto pCol = pObj->GetComp<Collider>())
    {
        pCol->SetIsPicking(false);
    }
}

void RayCaster::Update()noexcept
{
    m_pCurPickedObj = m_pCurMeshRayIntersectPickedObj = nullptr;

	if (KEY_TAP(GLFW_MOUSE_BUTTON_LEFT))
	{
      m_curRay = castRay();

      Mgr(ThreadMgr)->Enqueue(&RayCaster::UpdateMeshIntersectRay, this);

      const auto cache = m_vecPickingObjList.data();
      
      for (ushort i = 0; i < (const ushort)m_vecPickingObjList.size();)
      {
          shared_ptr<GameObj> pObj = cache[i].lock();
          if (pObj && pObj->IsAlive())
          {
              const auto pCol = pObj->GetComp<Collider>();
              Mgr(ThreadMgr)->Enqueue([this, pCol = pCol.get(), pObj = pObj.get()]()noexcept {
                  if (rayIntersectsOBB(m_curRay.rayDir, m_curRay.rayStart, pCol->GetOBB()))
                  {
                      std::lock_guard<SpinLock> lock{ m_spinLock };
                      setPickingObject.emplace(pObj);
                  }
                  });
              ++i;
          }
          else
          {
              cache[i].swap(m_vecPickingObjList.back());
              m_vecPickingObjList.pop_back();
          }
      }
      
      Mgr(ThreadMgr)->WaitAllJob();
      
      if (not setPickingObject.empty())
      {
          m_pCurPickedObj = (*setPickingObject.begin())->shared_from_this();
      }

      if (not setPickingObjectMesh.empty())
      {
          m_pCurMeshRayIntersectPickedObj = (*setPickingObjectMesh.begin()).first->shared_from_this();
          m_curIntersectPoint = (*setPickingObjectMesh.begin()).second;
      }

      setPickingObject.clear();
      setPickingObjectMesh.clear();
	}
}

void RayCaster::UpdateMeshIntersectRay() noexcept
{
    for (auto iter = m_mapMeshRayIntersectTable.begin(); iter != m_mapMeshRayIntersectTable.end();)
    {
        auto& [colTarget, flag_point] = *iter;
        auto& [flag, point] = flag_point;

        flag.store(false, std::memory_order_relaxed);
        if (colTarget->IsAlive())
        {
            const auto rootMat = colTarget->GetObjectWorldTransform();
            for (const auto& mesh : Mgr(ResMgr)->GetMeshList(colTarget->GetResName()) | std::views::values | std::views::join)
            {
               if (flag.load(std::memory_order_relaxed))
               {
                   break;
               }
               Mgr(ThreadMgr)->Enqueue([this, &mesh, &flag, rootMat, &colTarget,&point]()noexcept {
                   if (RayMeshIntersection(m_curRay, rootMat * mesh.meshWorldMat, *mesh.vertexPtr, *mesh.indexPtr, flag,point))
                   {
                       std::lock_guard<SpinLock> lock{ m_spinLockForMeshIntersect };
                       setPickingObjectMesh.emplace(colTarget.get(), point);
                   }
                   });
            }
            ++iter;
        }
        else
        {
            iter = m_mapMeshRayIntersectTable.erase(iter);
        }
    }
}

glm::vec2 RayCaster::normalizeDeviceCoordinates(const glm::vec2& vPos_) const noexcept
{
    const auto mPos = vPos_;
	const auto [width, height] = Mgr(Core)->GetWidthHeight();
	const float ndcX = (2.f * mPos.x) / width - 1.f;
	const float ndcY = 1.f - (2.f * mPos.y) / height;
	return glm::vec2{ ndcX,ndcY };
}

Ray RayCaster::castRay() const noexcept
{
    const auto ndc = normalizeDeviceCoordinates(Mgr(KeyMgr)->GetMousePos());
    const auto [invProjMatrix, invViewMatrix] = Camera::GetMainCamProjViewMatInv();

    const glm::vec4 rayStartClip(ndc.x, ndc.y, -1.f, 1.f);
    
    const glm::vec4 rayEndClip(ndc.x, ndc.y, 1.f, 1.f);   
    
    glm::vec4 rayStartView = invProjMatrix * rayStartClip;
    rayStartView /= rayStartView.w;
    glm::vec4 rayEndView = invProjMatrix * rayEndClip;
    rayEndView /= rayEndView.w;

    const glm::vec3 rayStartWorld = glm::vec3(invViewMatrix * rayStartView);
    const glm::vec3 rayEndWorld = glm::vec3(invViewMatrix * rayEndView);

    
    const glm::vec3 rayDirWorld = glm::normalize(rayEndWorld - rayStartWorld);

    return Ray{ rayDirWorld,rayStartWorld ,rayEndWorld};
}

const bool RayCaster::rayIntersectsOBB(const glm::vec3& rayDir, const glm::vec3& rayStart, const OBBBox& obb) const noexcept
{
    constexpr const double thresh_hold = 0.01;
    constexpr const double EPSILON = std::numeric_limits<double>::epsilon() + thresh_hold;

    double tMin = 0.0;
    double tMax = std::numeric_limits<double>::max();

    const glm::dvec3 obbCenter = static_cast<const glm::dvec3>(obb.getCenter());
    const glm::dvec3 delta = obbCenter - static_cast<const glm::dvec3>(rayStart);
    const glm::dvec3 actualExtents = static_cast<const glm::dvec3>(obb.getActualExtents());
    for (ushort i = 0; i < 3; ++i)
    {
        const glm::dvec3 axis = static_cast<const glm::dvec3>(obb.getAxis(i));
        const double e = glm::dot(axis, delta);
        const double f = glm::dot(static_cast<const glm::dvec3>(rayDir), axis);

        if (std::abs(f) > EPSILON)
        {
            double t1 = (e + actualExtents[i]) / f;
            double t2 = (e - actualExtents[i]) / f;

            if (t1 > t2) std::swap(t1, t2);

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax) return false;
        }
        else if (-e - actualExtents[i] > EPSILON || -e + actualExtents[i] < -EPSILON)
        {
            return false;
        }
    }
    return true;
}

const bool IntersectRayTriangle(const Ray& ray, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, glm::vec3& out)noexcept
{
    constexpr const float EPSILON = std::numeric_limits<float>::epsilon();
    const glm::vec3 edge1 = v1 - v0;
    const glm::vec3 edge2 = v2 - v0;
    const glm::vec3 h = glm::cross(ray.rayDir, edge2);
    const float a = glm::dot(edge1, h);

    if (a > -EPSILON && a < EPSILON)
        return false; 

    const float f = 1.0f / a;
    const glm::vec3 s = ray.rayStart - v0;
    const float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f)
        return false;

    const glm::vec3 q = glm::cross(s, edge1);
    const float v = f * glm::dot(ray.rayDir, q);
    if (v < 0.0f || u + v > 1.0f)
        return false;

    const float t = f * glm::dot(edge2, q);
    if (t > EPSILON)
    {
        out = ray.rayStart + ray.rayDir * t; 
        return true;
    }
    return false;
}

const bool RayMeshIntersection(const Ray& ray, const glm::mat4& modelMat,const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices,std::atomic_bool& flag, glm::vec3& out)noexcept
{   
    const auto num = indices.size();
    if (flag.load(std::memory_order_relaxed))
    {
        return false;
    }
    const Ray LocalRay = TransformRayToLocalSpace(ray, glm::inverse(modelMat));
    for (size_t i = 0; i < num; i += 3)
    {
        if (flag.load(std::memory_order_relaxed))
        {
            return false;
        }

        const glm::vec3 v0 = vertices[indices[i]].position;
        const glm::vec3 v1 = vertices[indices[i + 1]].position;
        const glm::vec3 v2 = vertices[indices[i + 2]].position;

        if (IntersectRayTriangle(LocalRay, v0, v1, v2,out))
        {
            flag.store(true,std::memory_order_relaxed);
            out = glm::vec3(modelMat * glm::vec4{ out, 1.0f });
            return true;  
        }
    }

    return false;  
}

const Ray TransformRayToLocalSpace(const Ray& ray, const glm::mat4& modelMatrixInverse) noexcept
{
    const glm::vec4 rayOriginLocal = modelMatrixInverse * glm::vec4(ray.rayStart, 1.0f);
    const glm::vec4 rayDirectionLocal = modelMatrixInverse * glm::vec4(ray.rayDir, 0.0f);
    return Ray(glm::normalize(glm::vec3(rayDirectionLocal)),glm::vec3(rayOriginLocal),{});
}