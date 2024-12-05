#include "pch.h"
#include "Object.h"
#include "Session.h"
#include "IOExecutor.h"
#include "Component.h"
#include "PathFollower.h"
#include "PacketBase.hpp"
#include "MCWorld.h"
#include "HP.h"

Object::Object()
	: m_obj_id{ IssueGlobalObjectID() }
	, m_session{}
{
}

Object::Object(std::shared_ptr<Session> session)
	: m_obj_id{ session->GetSessionID()}
	, m_session{ std::move(session) }
{
	InitHP<PlayerHP>(PLAYER_START_HP);
}

Object::~Object() noexcept
{
	//std::cout << "~Object\n";
	if (nullptr == m_session)
	{
		SendRemovePacket();
	}
}

void Object::Init() noexcept
{
	auto b = m_vecComp.data();
	const auto e = b + m_vecComp.size();
	while (e != b) { (*b++)->SetOwner(this); }
}

void Object::Update(const float DT) noexcept
{
	auto b = m_vecComp.data();
	const auto e = b + m_vecComp.size();
	while (e != b) { (*b++)->Update(DT); }
	if (nullptr == m_session && true == m_bDirtyFlag)
	{
		s2c_MOVE_OBJECT pkt;
		pkt.object_id = (uint32_t)m_obj_id;

		const auto pos = m_posInfo.m_vPos;
		const auto vel = m_posInfo.m_vVelocity;
		const auto accel = m_posInfo.m_vAccelation;
		const auto body_angle = m_posInfo.m_rendererBodyAngleY;

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
		pkt.yaw = body_angle;

		Mgr(MCWorld)->AppendToWorldSendBuffer(pkt);
	}
	else if (m_session)
	{
		
	}
	m_bDirtyFlag = false;
}

void Object::SetHP(const int hp_) noexcept
{
	m_HP->SetHP(hp_);
}

const int Object::GetHP() const noexcept
{
	return m_HP->GetHP();
}

void Object::IncHP(const int inc_) noexcept
{
	m_HP->IncHP(inc_);
}

void Object::DecHP(const int dec_) noexcept
{
	m_HP->DecHP(dec_);
}

void Object::SendRemovePacket() const noexcept
{
	s2c_REMOVE_OBJECT pkt;
	pkt.object_id = GetObjectID();
	pkt.obj_type = (uint8)GetObjectType();
	Mgr(MCWorld)->AppendToWorldSendBuffer(pkt);
}

void Object::SetUpHP(const int hp_)noexcept
{
	m_HP->SetHP(hp_);
	m_HP->SetOwner(this);
}
