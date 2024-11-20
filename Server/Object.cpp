#include "pch.h"
#include "Object.h"
#include "Session.h"
#include "IOExecutor.h"
#include "Component.h"
#include "EntityMovement.h"
#include "PacketBase.hpp"
#include "MCWorld.h"

Object::Object()
	: m_obj_id{ IssueGlobalObjectID() }
	, m_session{}
	, m_pEntityMovemet{ (EntityMovement*)AddComp(new EntityMovement).get() }
{
}

Object::Object(std::shared_ptr<Session> session)
	: m_obj_id{ session->GetSessionID()}
	, m_session{ std::move(session) }
	, m_pEntityMovemet{ (EntityMovement*)AddComp(new EntityMovement).get() }
{
}

void Object::Update(const float DT) noexcept
{
	auto b = m_vecComp.data();
	const auto e = b + m_vecComp.size();
	while (e != b) { (*b++)->Update(DT); }
	if (nullptr == m_session && true == m_bDirtyFlag)
	{
		s2c_MOVE_OBJECT pkt;
		const auto pos = m_pEntityMovemet->current_position;
		const auto vel = m_pEntityMovemet->m_vVelocity;
		const auto accel = m_pEntityMovemet->m_vAccelation;
		const auto body_angle = m_pEntityMovemet->m_rendererBodyAngleY;

		pkt.position_x = pos.x;
		pkt.position_y = pos.y;
		pkt.position_z = pos.z;

		pkt.velocity_x = vel.x;
		pkt.velocity_y = vel.y;
		pkt.velocity_z = vel.z;

		pkt.acceleration_x = accel.x;
		pkt.acceleration_y = accel.y;
		pkt.acceleration_z = accel.z;

		pkt.rotation_y = body_angle;

		Mgr(MCWorld)->AppendToWorldSendBuffer(pkt);
	}
	m_bDirtyFlag = false;
}
