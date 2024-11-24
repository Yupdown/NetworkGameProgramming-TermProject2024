#include "pch.h"
#include "Monster.h"
#include "Transform.h"
#include "AssimpMgr.h"

void Monster::Update()
{
	Player::HandleCollision();

	UpdatePlayerCamFpsMode();
	UpdateCameraTransform(m_pCacheMyTransformCamera);

	GameObj::Update();
	UpdateRenderer();


}

void Monster::InitializeRenderer()
{
	m_rendererObj = Mgr(AssimpMgr)->LoadAllPartsAsGameObj("DefaultWarpShader.glsl", "Monster.fbx");

	m_transformHead = m_rendererObj->FindChildObj("Enderman.001")->GetTransform();
	m_transformLArm = m_rendererObj->FindChildObj("Enderman.002")->GetTransform();
	m_transformRArm = m_rendererObj->FindChildObj("Enderman.005")->GetTransform();
	m_transformLLeg = m_rendererObj->FindChildObj("Enderman.003")->GetTransform();
	m_transformRLeg = m_rendererObj->FindChildObj("Enderman.004")->GetTransform();
}

void Monster::UpdateRenderer()
{
	m_rendererObj->GetTransform()->SetLocalScale(0.02f);

	const float rotationFactor = glm::min(glm::length(m_vVelocity) * glm::pi<float>() * 0.05f, 0.5f) * sin(m_fMoveTime * 2.0f);
	const float l = 30.0f;
	const float dx = l * sin(rotationFactor);
	const float dy = l * (1.0f - cos(rotationFactor));

	// Get angle from its acceleration
	if (glm::length(m_vAccelation) > 0.1f)
	{
		float angleTarget = atan2(m_vAccelation.x, m_vAccelation.z) * F_RAD2DEG;
		m_rendererBodyAngleY = LerpAngle(m_rendererBodyAngleY, angleTarget, DT * 8.0f);
		UpdateBodyRotation(m_rendererBodyAngleY - 90.0f);
		UpdateHeadRotation(0.0f, m_rendererBodyAngleY);
	}

	m_transformLArm->SetLocalPosition(glm::vec3(m_transformLArm->GetLocalPosition().x, 76.2f + dy, -dx));
	m_transformRArm->SetLocalPosition(glm::vec3(m_transformRArm->GetLocalPosition().x, 76.2f + dy, dx));
	m_transformLArm->SetLocalRotation(glm::quat(glm::vec3(rotationFactor + glm::half_pi<float>(), 0.0f, 0.0f)));
	m_transformRArm->SetLocalRotation(glm::quat(glm::vec3(-rotationFactor + glm::half_pi<float>(), 0.0f, 0.0f)));
	m_transformLLeg->SetLocalRotation(glm::quat(glm::vec3(-rotationFactor + glm::half_pi<float>(), 0.0f, 0.0f)));
	m_transformRLeg->SetLocalRotation(glm::quat(glm::vec3(rotationFactor + glm::half_pi<float>(), 0.0f, 0.0f)));
}
