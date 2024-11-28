#include "pch.h"
#include "DeathObject.h"
#include <EventHandler.h>
#include <Transform.h>

DeathObject::DeathObject(shared_ptr<GameObj> renderObj) : m_renderObj(renderObj), GameObj()
{
}

void DeathObject::Start()
{
	GameObj::Start();

	AddChild(m_renderObj);
}

void DeathObject::Update()
{
	m_fLifeTime += DT;

	auto transform = m_renderObj->GetTransform();
	transform->SetLocalRotation(glm::angleAxis(glm::radians(glm::sin(m_fLifeTime * glm::half_pi<float>()) * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

	if (m_fLifeTime > 1.0f)
	{
		DestroyObj(shared_from_this());
		return;
	}

	GameObj::Update();
}
