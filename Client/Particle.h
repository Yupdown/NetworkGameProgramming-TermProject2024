#pragma once
#include "GameObj.h"

class GameObj;
class MeshRenderer;

class Particle
	:public GameObj
{
	static constexpr float g_particleSpeed = 5.0f;
	static std::unordered_map<string, shared_ptr<MeshRenderer>> g_mapMeshRenderer;
private:
	string m_strParticleResName;
	bool m_bIsActivate = false;
	float m_fLife = 0.f;
public:
	Particle()noexcept;
	~Particle();
	Particle(const Particle&) = delete;
	Particle operator = (const Particle&) = delete;
	void ActivateParticle(shared_ptr<MeshRenderer> pMeshRenderer,string_view strResName_,const glm::vec3& worldPos_,const glm::vec3& scale_)noexcept;
	bool IsActivate()const { return m_bIsActivate; }
	void Update() override;
};

