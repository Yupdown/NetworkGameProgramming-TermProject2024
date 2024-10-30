#pragma once
#include "pch.h"

struct Ray
{
	glm::vec3 rayDir;
	glm::vec3 rayStart;
	glm::vec3 rayEnd;
	Ray()noexcept = default;
	Ray(const glm::vec3& dir_,const glm::vec3& start_,const glm::vec3& end_)noexcept
		:rayDir{dir_},rayStart{start_},rayEnd{end_}{}
};

class GameObj;
struct OBBBox;
class Collider;


const bool IntersectRayTriangle(const Ray& ray,const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,glm::vec3& out)noexcept;
const bool RayMeshIntersection(const Ray& ray, const glm::mat4& modelMat, const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, std::atomic_bool& flag,glm::vec3& out)noexcept;
const Ray TransformRayToLocalSpace(const Ray& ray, const glm::mat4& modelMatrixInverse) noexcept;

class RayCaster
	:public Singleton<RayCaster>
{
	friend class Singleton;
	RayCaster();
	~RayCaster();
private:
	shared_ptr<GameObj> m_pCurPickedObj;
	shared_ptr<GameObj> m_pCurMeshRayIntersectPickedObj;
	glm::vec3 m_curIntersectPoint = {};
	vector<weak_ptr<GameObj>> m_vecPickingObjList;
	std::unordered_map<shared_ptr<GameObj>, std::pair<std::atomic_bool,glm::vec3>> m_mapMeshRayIntersectTable;
	Ray m_curRay;
	SpinLock m_spinLock;
	SpinLock m_spinLockForMeshIntersect;
private:
	void UpdateMeshIntersectRay()noexcept;
public:
	void Init();
	void Update()noexcept;
	void RegisterOBBPickingList(weak_ptr<GameObj> pCol_)noexcept;
	glm::vec2 normalizeDeviceCoordinates(const glm::vec2& vPos_)const noexcept;
	Ray castRay()const noexcept;
	const bool rayIntersectsOBB(const glm::vec3& rayDir, const glm::vec3& rayStart, const OBBBox& obb)const noexcept;
	const shared_ptr<GameObj>& GetCurPickedObj()const noexcept{ return m_pCurPickedObj; }
	void RegisterMeshRayIntersect(const shared_ptr<GameObj>& pObj)noexcept;
	const shared_ptr<GameObj>& GetCurPickedMeshRayIntersectObj()const noexcept { return m_pCurMeshRayIntersectPickedObj; }

	const auto GetPickedObjAndPoint()const noexcept { return std::make_pair(m_pCurMeshRayIntersectPickedObj, m_curIntersectPoint); }
};

