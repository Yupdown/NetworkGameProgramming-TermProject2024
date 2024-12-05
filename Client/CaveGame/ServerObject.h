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
	/// ������Ʈ�� ����ü�� ���� ������ ������ �޾��� �� Ŭ���̾�Ʈ���� �̺�Ʈ�� ó���� �� �ֵ��� �ϴ� �Լ�
	/// </summary>
	/// <param name="value">
	/// ������ ���� ���� ü��
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