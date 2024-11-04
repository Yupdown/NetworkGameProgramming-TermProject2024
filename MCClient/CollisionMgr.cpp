#include "pch.h"
#include "CollisionMgr.h"
#include "Collider.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "GameObj.h"
#include "ThreadMgr.h"
#include "Transform.h"
#include "Camera.h"
#include "ResMgr.h"
#include "KeyMgr.h"

const float projectOntoAxis(const OBBBox& box, const glm::vec3& axis) noexcept
{
	const auto corners = box.GetCorners();

	float minProj = std::numeric_limits<float>::max();
	float maxProj = std::numeric_limits<float>::min();

	const auto cache = corners.data();
	const ushort num = (const ushort)corners.size();

	for (ushort i = 0; i < num; ++i)
	{
		const float proj = glm::dot(axis, cache[i]);
		minProj = glm::min(minProj, proj);
		maxProj = glm::max(maxProj, proj);
	}

	return (maxProj - minProj) * 0.5f;
}

bool testAxis(const glm::vec3& axis, const OBBBox& box1, const OBBBox& box2, float& outOverLap) noexcept
{
	const float proj1 = projectOntoAxis(box1, axis);
	const float proj2 = projectOntoAxis(box2, axis);
	const float distance = glm::abs(glm::dot(box2.getCenter() - box1.getCenter(), axis));

	return std::numeric_limits<float>::epsilon() < (outOverLap = (proj1 + proj2) - distance);
}

void CollisionInfo::IsCollision() noexcept
{
	const OBBBox& box1 = colA->GetOBB();
	const OBBBox& box2 = colB->GetOBB();

	glm::vec3 axes[15] = {
		box1.getAxis(0),
		box1.getAxis(1),
		box1.getAxis(2),
		box2.getAxis(0),
		box2.getAxis(1),
		box2.getAxis(2)
	};

	ushort axesNum = 6;
	for (ushort i = 0; i < 3; ++i)
	{
		for (ushort j = 0; j < 3; ++j)
		{
			const glm::vec3 crossAxis = glm::cross(box1.getAxis(i), box2.getAxis(j));
			if (!IsZeroVector(crossAxis))
			{
				axes[axesNum++] = glm::normalize(crossAxis);
			}
		}
	}

	float smallestOverlap = std::numeric_limits<float>::infinity();
	glm::vec3 smallestAxis;

	for (unsigned short i = 0; i < axesNum; ++i)
	{
		float overlap;

		if (!testAxis(axes[i], box1, box2, overlap))
		{
			bNowCollision = false;
			return;
		}
		else
		{
			if (overlap < smallestOverlap)
			{
				smallestOverlap = overlap;
				smallestAxis = axes[i];
			}
		}
	}

	bNowCollision = true;

	const glm::vec3 penetrationVector = smallestAxis * smallestOverlap;
	this->penetrationVector = glm::dot(penetrationVector, box2.getCenter() - box1.getCenter()) < 0 ? -penetrationVector : penetrationVector;
}


CollisionMgr::CollisionMgr()
{
	m_mapCollision.reserve(1024);
}

CollisionMgr::~CollisionMgr()
{
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteVertexArrays(1, &vao);
}

void CollisionMgr::Init()
{
	m_colShader = Mgr(ResMgr)->GetRes<Shader>("CollisionBoxShader.glsl");
	m_colShader->Use();
	m_arrCollisionBox = std::array<glm::vec3, 8> {
		glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.5f, 0.0f, -0.5f),
			glm::vec3(0.5f, 1.0f, -0.5f), glm::vec3(-0.5f, 1.0f, -0.5f),
			glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec3(0.5f, 0.0f, 0.5f),
			glm::vec3(0.5f, 1.0f, 0.5f), glm::vec3(-0.5f, 1.0f, 0.5f)
	};
	m_indices = std::array<GLuint, 24>{
		0, 1, 1, 2, 2, 3, 3, 0,
			4, 5, 5, 6, 6, 7, 7, 4,
			0, 4, 1, 5, 2, 6, 3, 7
	};
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, m_arrCollisionBox.size() * sizeof(glm::vec3), m_arrCollisionBox.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), m_indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

}

void CollisionMgr::CollisionUpdate()
{
	for (int iRow = 0; iRow < etoi(GROUP_TYPE::END); ++iRow)
	{
		for (int iCol = iRow; iCol < etoi(GROUP_TYPE::END); ++iCol)
		{
			if (m_bitColTable[iRow][iCol])
			{
				Mgr(ThreadMgr)->Enqueue(&CollisionMgr::CollisionUpdateGroup, this, static_cast<GROUP_TYPE>(iRow), static_cast<GROUP_TYPE>(iCol));
			}
		}
	}

	Mgr(ThreadMgr)->WaitAllJob();

	const auto cache = m_vecCollisionInfo.data();

	for (unsigned short i = 0; i < (const unsigned short)m_vecCollisionInfo.size();)
	{
		if (cache[i]->bNowCollision)
		{
			if (cache[i]->bPrevCollision)
			{
				if (cache[i]->colA->IsDead() || cache[i]->colB->IsDead())
				{
					cache[i]->colA->OnCollisionExit(cache[i]->colB);
					cache[i]->colB->OnCollisionExit(cache[i]->colA);
					m_mapCollision.erase(cache[i]->collision_ID);
					cache[i] = m_vecCollisionInfo.back();
					m_vecCollisionInfo.pop_back();
					continue;
				}
				else
				{
					cache[i]->colA->OnCollisionStay(cache[i]->colB);
					cache[i]->colB->OnCollisionStay(cache[i]->colA);
				}
			}
			else
			{
				if (!cache[i]->colA->IsDead() && !cache[i]->colB->IsDead())
				{
					cache[i]->colA->OnCollisionEnter(cache[i]->colB);
					cache[i]->colB->OnCollisionEnter(cache[i]->colA);
					cache[i]->bPrevCollision = true;
				}
				else
				{
					m_mapCollision.erase(cache[i]->collision_ID);
					cache[i] = m_vecCollisionInfo.back();
					m_vecCollisionInfo.pop_back();
					continue;
				}
			}
		}
		else
		{
			if (cache[i]->bPrevCollision)
			{
				cache[i]->colA->OnCollisionExit(cache[i]->colB);
				cache[i]->colB->OnCollisionExit(cache[i]->colA);
				cache[i]->bPrevCollision = false;
			}
			if (cache[i]->colA->IsDead() || cache[i]->colB->IsDead())
			{
				m_mapCollision.erase(cache[i]->collision_ID);
				cache[i] = m_vecCollisionInfo.back();
				m_vecCollisionInfo.pop_back();
				continue;
			}
		}
		++i;
	}
	if (KEY_TAP(GLFW_KEY_SEMICOLON))
	{
		m_bRenderCollisionBox = !m_bRenderCollisionBox;
	}
}

void CollisionMgr::RegisterGroup(GROUP_TYPE _eLeft, GROUP_TYPE _eRight)
{
	int iRow = static_cast<int>(_eLeft);
	int iCol = static_cast<int>(_eRight);

	if (iRow > iCol)
	{
		std::swap(iRow, iCol);
	}

	m_bitColTable[iRow][iCol].flip();
}

void CollisionMgr::RenderCollisionBox()
{
	if (!m_bRenderCollisionBox)
	{
		return;
	}

	const auto curCam = Camera::GetCurCam();
	const auto proj = curCam->GetCamMatProj();
	const auto view = curCam->GetCamMatView();
	auto curScene = Mgr(SceneMgr)->GetCurScene();
	m_colShader->Use();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for (int i = 0; i < etoi(GROUP_TYPE::END); ++i)
	{
		for (const auto& objs : curScene->GetGroupObj((GROUP_TYPE)(i)))
		{
			for (const auto& p : *objs)
			{
				if (const auto pCol = p->GetComp<Collider>())
				{
					const auto finalMat = proj * view * pCol->GetOBB().getFinalMatrix();
					m_colShader->SetUniformMat4(finalMat, "uModelViewProjectionMatrix");

					glBindVertexArray(vao);
					glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
					glBindVertexArray(0);
				}
			}
		}
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

const glm::vec3& CollisionMgr::GetPenetrationVector(shared_ptr<Collider>& a_, shared_ptr<Collider>& b_) const noexcept
{
	COLLIDER_ID id;
	id.Left_id = a_->GetID();
	id.Right_id = b_->GetID();

	const auto iter = m_mapCollision.find(id.ID);
	if (nullptr != iter)
	{
		return iter->second.penetrationVector;
	}
	else
	{
		std::swap(id.Left_id, id.Right_id);
		return m_mapCollision.find(id.ID)->second.penetrationVector;
	}
}

void CollisionMgr::CollisionUpdateGroup(GROUP_TYPE _eLeft, GROUP_TYPE _eRight) noexcept
{
	const auto pCurScene = Mgr(SceneMgr)->GetCurScene();

	const auto& vecLeft = pCurScene->GetGroupObj(_eLeft);
	const auto& vecRight = pCurScene->GetGroupObj(_eRight);

	const auto vecLeftCache = vecLeft.data();
	const ushort vecLeftSize = (const ushort)vecLeft.size();

	const auto vecRightCache = vecRight.data();
	const ushort vecRightSize = (const ushort)vecRight.size();

	for (ushort i = 0; i < vecLeftSize; ++i)
	{
		for (const auto& pLeftObj : *vecLeftCache[i])
		{
			const auto pLeftCol = pLeftObj->GetComp<Collider>();

			if (!pLeftCol)
			{
				continue;
			}

			for (ushort j = 0; j < vecRightSize; ++j)
			{
				for (const auto& pRightObj : *vecRightCache[j])
				{
					const auto pRightCol = pRightObj->GetComp<Collider>();

					if (!pRightCol || pLeftCol == pRightCol)
					{
						continue;
					}

					COLLIDER_ID ID;

					ID.Left_id = pLeftCol->GetID();
					ID.Right_id = pRightCol->GetID();

					//const auto [iter, insert] = m_mapCollision.try_emplace(ID.ID, CollisionInfo{ pLeftCol,pRightCol ,ID.ID });
					//auto iter = m_mapCollision.find(ID.ID);
					//auto iter = m_mapCollision.find(ID.ID);
					const auto [iter,insert] = m_mapCollision.try_emplace(ID.ID, pLeftCol, pRightCol, ID.ID);
					Mgr(ThreadMgr)->Enqueue(&CollisionInfo::IsCollision, &iter->second);
					if (insert)
					{
						std::lock_guard<SpinLock> lock{ m_spinLock };
						m_vecCollisionInfo.emplace_back(&iter->second);
					}
					//if (nullptr == iter)
					//{
					//	iter = m_mapCollision.emplace(ID.ID, CollisionInfo{ pLeftCol,pRightCol ,ID.ID });
					//	Mgr(ThreadMgr)->Enqueue(&CollisionInfo::IsCollision, &iter->second);
					//	std::lock_guard<SpinLock> lock{ m_spinLock };
					//	m_vecCollisionInfo.emplace_back(&iter->second);
					//}
					//else
					//{
					//	Mgr(ThreadMgr)->Enqueue(&CollisionInfo::IsCollision, &iter->second);
					//}


					//++m_iNumOfCollision;

					//Mgr(ThreadMgr)->Enqueue(&CollisionInfo::IsCollision, &iter->second);
				}
			}
		}
	}
}