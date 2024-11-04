#include "pch.h"
#include "RigidBody.h"
#include "TimeMgr.h"
#include "Transform.h"

RigidBody::RigidBody()
	:Component{COMPONENT_TYPE::RIGIDBODY}
{
}

RigidBody::~RigidBody()
{
}

void RigidBody::UpdatePhysics() noexcept
{
	m_vAccel = m_vForce / m_fMass;
	m_vVelocity += m_vAccel * DT;
	if (!IsZeroVector(m_vVelocity))
	{
		m_vDir = glm::normalize(m_vVelocity);
		const glm::vec3 vFriction = m_vDir * m_fFriction * DT;
		if (glm::length(m_vVelocity) <= glm::length(vFriction))
		{
			m_vVelocity = ZERO_VEC3;
		}
		else
		{
			m_vVelocity -= vFriction;
		}
	}
	const float x_max = bitwise_absf(m_vMaxVelocity.x);
	const float y_max = bitwise_absf(m_vMaxVelocity.y);
	const float z_max = bitwise_absf(m_vMaxVelocity.z);

	const float x_vel = bitwise_absf(m_vVelocity.x);
	const float y_vel = bitwise_absf(m_vVelocity.y);
	const float z_vel = bitwise_absf(m_vVelocity.z);

	if (x_max < x_vel)
	{
		m_vVelocity.x = m_vVelocity.x / x_vel * x_max;
	}
	if (y_max < y_vel)
	{
		m_vVelocity.y = m_vVelocity.y / y_vel * y_max;
	}
	if (z_max < z_vel)
	{
		m_vVelocity.z = m_vVelocity.z / z_vel * z_max;
	}

	UpdateLimit();

	Move();
	m_vForce = ZERO_VEC3;

	m_bDirty = false;
}

void RigidBody::UpdateGravity() noexcept
{
	if (m_bGravity && !m_bIsGround)
	{
		AddForce(glm::vec3{ 0.0f,-3.0f * 2.f,0.f } + m_vGravityOffset);
	}
}

void RigidBody::Move() noexcept
{
	if (IsZeroVector(m_vVelocity))
	{
		return;
	}
	if (m_bIsGround)
	{
		m_vVelocity.y = 0.f;
	}
	GetTransform()->AddPosition(m_vVelocity * DT);
}

void RigidBody::LastUpdate()
{
	UpdateGravity();
	UpdatePhysics();
}

void RigidBody::FinalUpdate()
{
	const auto pTrans = GetTransform();
	//if (m_bGravity &&!m_bIsGround && pTrans->GetWorldPositionAccRecursion().y < 0.f)
	//{
	//	pTrans->ResetY();
	//	m_bIsGround = true;
	//}
	if (!m_bDirty)
	{
		return;
	}
	UpdatePhysics();
}
