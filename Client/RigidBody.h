#pragma once
#include "pch.h"
#include "Component.h"

class RigidBody
	:public Component
{
private:
	glm::vec3 m_vForce = {};
	glm::vec3 m_vAccel = {};
	glm::vec3 m_vVelocity = {};
	glm::vec3 m_vDir = {};
	glm::vec3 m_vMaxVelocity = glm::vec3{ 1.5f,30.5f,1.5f };
	glm::ivec3 m_ivLimit = glm::ivec3{};
	glm::vec3 m_vGravityOffset = {};
	float m_fMass = 1.f;
	float m_fFriction = .8f;
	bool m_bGravity = true;
	bool m_bIsGround = false;
	bool m_bDirty = true;
	bool m_bIsLimit = false;
public:
	RigidBody();
	~RigidBody();
	COMP_CLONE(RigidBody)
private:
	void UpdatePhysics()noexcept;
	void UpdateGravity()noexcept;
	void Move()noexcept;
	void UpdateLimit()noexcept
	{
		if (0 != m_ivLimit.x)
		{
			if (0.f < (float)m_ivLimit.x * m_vVelocity.x)
			{
				m_vVelocity.x = 0.f;
			}
		}
		if (0 != m_ivLimit.y)
		{
			if (0.f < (float)m_ivLimit.y * m_vVelocity.y)
			{
				m_vVelocity.y = 0.f;
			}
		}
		if (0 != m_ivLimit.z)
		{
			if (0.f < (float)m_ivLimit.z * m_vVelocity.z)
			{
				m_vVelocity.z = 0.f;
			}
		}
	}
public:
	glm::vec3 GetDir()const noexcept { return m_vDir; }
	void SetLimitFlag(const glm::vec3& cannotMoveDir = glm::vec3{}) noexcept {
		m_ivLimit = glm::ivec3{
			!IsFloatZero(cannotMoveDir.x) ? (int)glm::sign(cannotMoveDir.x) : 0,
			!IsFloatZero(cannotMoveDir.y) ? (int)glm::sign(cannotMoveDir.y) : 0,
			!IsFloatZero(cannotMoveDir.z) ? (int)glm::sign(cannotMoveDir.z) : 0
		};
	}
	bool  IsGround()const { return m_bIsGround || IsFloatZero(m_vVelocity.y); }
	void SetIsGround(bool _b)
	{
		m_bIsGround = _b;
		m_bDirty = true;
		if (m_bIsGround)
		{
			m_vForce.y = 0.f;
			m_vVelocity.y = 0.f;
		}
	}
	void SetGravityOffset(const glm::vec3 _vGravityOffset)noexcept { m_vGravityOffset = _vGravityOffset; m_bDirty = true; }
	void SetLimit(const glm::vec3 _vLimit)noexcept { m_vMaxVelocity = _vLimit; }
	bool  IsGravity()const { return m_bGravity; }
	void  SetGravity(bool _b) { m_bGravity = _b; m_bDirty = true;}
	constexpr inline glm::vec3 GetVelocity()const { return m_vVelocity; }
	constexpr inline void SetVelocity(glm::vec3 _v) { m_vVelocity = _v; m_bDirty = true; }
	
	constexpr inline void SetFriction(float _value) { m_fFriction = _value; m_bDirty = true;}

	constexpr inline float GetFriction() const { return m_fFriction; }
	constexpr inline void SetMass(float _fMass) { m_fMass = _fMass; m_bDirty = true;}
	constexpr inline float GetMass()const { return m_fMass; }

	constexpr inline void AddForce(glm::vec3 _vF) { m_vForce += _vF; m_bDirty = true;}
	constexpr inline void AddVelocity(glm::vec3 _v) { m_vVelocity += _v; m_bDirty = true;}
	constexpr inline glm::vec3 GetForce()const { return m_vForce; }
	constexpr inline void SetForce(glm::vec3 _v) { m_vForce = _v; m_bDirty = true;}
public:
	void LastUpdate()override;
	void FinalUpdate()override;
};

