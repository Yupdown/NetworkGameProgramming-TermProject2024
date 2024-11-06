#include "pch.h"
#include "ParticleMgr.h"
#include "Core.h"
#include "Transform.h"
#include "MeshRenderer.h"

ParticleMgr::ParticleMgr()noexcept
{
	for (auto& particle : m_arrParticles)
	{
		particle = make_shared<Particle>();
	}
}

ParticleMgr::~ParticleMgr()
{
}

void ParticleMgr::Init()
{
}

void ParticleMgr::SetParticles(const shared_ptr<GameObj>& pObj_, const float fScaleRatio,const glm::vec3& worldPos)noexcept
{
	const unsigned short end_particles = m_iCurParticleNum + PARTICE_STEP;
	const auto cache = m_arrParticles.data();
	const auto pTrans = pObj_->GetTransform();
	string_view strResName = pObj_ -> GetResName();
	const auto mr = pObj_->GetComp<MeshRenderer>();
	const glm::vec3 particlePos = IsZeroVector(worldPos) ? pTrans->GetWorldPosition() : worldPos;
	const glm::vec3 particleScale = pTrans->GetLocalScale() * fScaleRatio;
	for (unsigned short i = m_iCurParticleNum; i < end_particles; ++i)
	{
		cache[i]->ActivateParticle(mr, strResName, particlePos, particleScale);
	}
	m_iCurParticleNum = (m_iCurParticleNum + PARTICE_STEP) % MAX_PARTICLE;
}