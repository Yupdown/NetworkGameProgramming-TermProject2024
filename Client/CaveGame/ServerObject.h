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
	glm::vec3 GetVelocity() const { return m_vVelocity; }
	glm::vec3 GetAcceleration() const { return m_vAccelation; }

	void SetPosition(const glm::vec3& position) { GetTransform()->SetLocalPosition(position); }
	void SetRotation(const glm::quat& rotation) { GetTransform()->SetLocalRotation(rotation); }
	void SetVelocity(const glm::vec3& velocity) { m_vVelocity = velocity; }
	void SetAcceleration(const glm::vec3& acceleration) { m_vAccelation = acceleration; }

	void SetID(const int id_) { m_id = id_; }

	/// <summary>
	/// 오브젝트가 투사체나 근접 등으로 공격을 받았을 때 클라이언트에서 이벤트를 처리할 수 있도록 하는 함수
	/// </summary>
	/// <param name="value">
	/// 공격을 받은 후의 체력
	/// </param>
	virtual void OnObjectDamaged(int value);

protected:
	float m_lookYaw = 0.0f;
	float m_lookPitch = 0.0f;
	glm::vec3 m_vVelocity = glm::zero<glm::vec3>();
	glm::vec3 m_vAccelation = glm::zero<glm::vec3>();
private:
	unsigned int m_id = 0;
};