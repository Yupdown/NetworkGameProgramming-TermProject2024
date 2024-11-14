#pragma once

#include "pch.h"

#include <GameObj.h>
#include <PacketBase.hpp>
#include <Transform.h>

class ServerObject 
	: public GameObj
{
public:
	~ServerObject() noexcept;

	void SyncMovement(const s2c_MOVE_OBJECT& pkt);

public:
	unsigned int GetID() const { return m_id; }

	glm::vec3 GetPosition() const { return GetTransform()->GetLocalPositionAcc(); }
	glm::quat GetRotation() const { return GetTransform()->GetLocalRotationAcc(); }
	glm::vec3 GetVelocity() const { return m_velocity; }
	glm::vec3 GetAcceleration() const { return m_acceleration; }

	void SetPosition(const glm::vec3& position) { GetTransform()->SetLocalPosition(position); }
	void SetRotation(const glm::quat& rotation) { GetTransform()->SetLocalRotation(rotation); }
	void SetVelocity(const glm::vec3& velocity) { m_velocity = velocity; }
	void SetAcceleration(const glm::vec3& acceleration) { m_acceleration = acceleration; }

	void SetID(const int id_) { m_id = id_; }
private:
	unsigned int m_id = 0;

	glm::vec3 m_velocity = glm::zero<glm::vec3>();
	glm::vec3 m_acceleration = glm::zero<glm::vec3>();
};