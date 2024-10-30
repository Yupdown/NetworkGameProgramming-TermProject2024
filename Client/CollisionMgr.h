#pragma once
#include "pch.h"
#include "Singleton.hpp"
#include "ConcurrentHash.hpp"

class Collider;
class Shader;
struct OBBBox;

bool testAxis(const glm::vec3& axis, const OBBBox& box1, const OBBBox& box2, float& outOverLap) noexcept;
const float projectOntoAxis(const OBBBox& box, const glm::vec3& axis)noexcept;


union COLLIDER_ID
{
	struct{
		GLuint Left_id;
		GLuint Right_id;
	};
	ull ID;
};

struct CollisionInfo
{
	bool bPrevCollision = false;
	bool bNowCollision = false;
	shared_ptr<Collider> colA;
	shared_ptr<Collider> colB;
	ull collision_ID;
	glm::vec3 penetrationVector;
	CollisionInfo()noexcept = default;
	CollisionInfo(shared_ptr<Collider> a_, shared_ptr<Collider> b_,const ull colID_)noexcept
		:colA{ std::move(a_) }, colB{ std::move(b_) }, collision_ID{colID_} {}
	void IsCollision() noexcept;
};

class CollisionMgr
	:public Singleton<CollisionMgr>
{
	friend class Singleton;
	CollisionMgr();
	~CollisionMgr();
private:
	//unordered_map<ull, CollisionInfo> m_mapCollision;
	ConcurrentHashMap<ull, CollisionInfo> m_mapCollision;
	vector<CollisionInfo*> m_vecCollisionInfo;
	std::bitset<etoi(GROUP_TYPE::END)> m_bitColTable[etoi(GROUP_TYPE::END)];
	std::array<glm::vec3, 8> m_arrCollisionBox;
	std::array<GLuint, 24> m_indices;
	shared_ptr<Shader> m_colShader;
	GLuint vao, vbo,ebo;
	bool m_bRenderCollisionBox = false;
	SpinLock m_spinLock = {};
private:
	void CollisionUpdateGroup(GROUP_TYPE _eLeft, GROUP_TYPE _eRight)noexcept;
public:
	void Init();
	void CollisionUpdate();
	void RegisterGroup(GROUP_TYPE _eLeft, GROUP_TYPE _eRight);
	void Reset()
	{
		m_mapCollision.clear();
		m_vecCollisionInfo.clear();
		for (auto& bit : m_bitColTable)
		{
			bit.reset();
		}
	}
	void RenderCollisionBox();
	const glm::vec3& GetPenetrationVector(shared_ptr<Collider>& a_, shared_ptr<Collider>& b_)const noexcept;
};
