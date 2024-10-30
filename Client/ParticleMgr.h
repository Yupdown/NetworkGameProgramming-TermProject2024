#pragma once
#include "Particle.h"

class GameObj;

class ParticleMgr
	:public Singleton<ParticleMgr>
{
	static constexpr uint MAX_PARTICLE = 10'0000;
	static constexpr uint PARTICE_STEP = 100;
	friend class Singleton;
	ParticleMgr()noexcept;
	~ParticleMgr();
private:
	array<shared_ptr<Particle>, MAX_PARTICLE> m_arrParticles;
	ushort m_iCurParticleNum = 0;
public:
	void Init();
	void SetParticles(const shared_ptr<GameObj>& pObj_, const float fScaleRatio = 0.01f,const glm::vec3& worldPos = glm::vec3{})noexcept;
};

