#include "pch.h"
#include "EnderDragon.h"
#include "EnderDragonRenderer.h"
#include <UIMgr.h>
#include <KeyMgr.h>

EnderDragon::EnderDragon()
{

}

void EnderDragon::Start()
{
	m_renderDragonRenderer = std::make_shared<EnderDragonRenderer>();
	AddChild(m_renderDragonRenderer);

	Mgr(UIMgr)->SetBossHealthActive(true);

	ServerObject::Start();
}

void EnderDragon::Update()
{
	m_rendererBodyAngleY = glm::degrees(-std::atan2(m_vVelocity.z, m_vVelocity.x));

	const glm::quat bodyRotation = glm::quat(glm::vec3(0.0f, glm::radians(m_rendererBodyAngleY - 90.f), 0.0f));
	GetTransform()->SetLocalRotation(bodyRotation);
	
	ServerObject::Update();
}

void EnderDragon::OnObjectDamaged(int value)
{
	Mgr(UIMgr)->SetBossHealth(value);

	if (value <= 0)
	{
		Mgr(UIMgr)->SetBossHealthActive(false);
		Mgr(KeyMgr)->SetMouseMode(GLFW_CURSOR_NORMAL);
		Mgr(UIMgr)->SetGameClearPanelActive(true);
	}
}
