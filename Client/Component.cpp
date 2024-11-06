#include "pch.h"
#include "Component.h"
#include "GameObj.h"
#include "Transform.h"

void Component::MarkTransformDirty() noexcept
{
	m_bTransformDirty = GetTransform()->IsDirty();
}

Component::Component(COMPONENT_TYPE _eType)
	:Resource{RESOURCE_TYPE::COMPONENT}
	,m_eCompType{_eType}
{
}

Component::Component(const Component& _other)
	:Resource{_other}
	,m_eCompType{_other.m_eCompType}
	//,m_waitLock{}
{
}

Component::~Component()
{
}

shared_ptr<GameObj> Component::GetGameObj() const
{
	return m_pOwner.lock();
}

shared_ptr<Transform> Component::GetTransform() const noexcept
{
	return m_pOwner.lock()->GetTransform();
}

const bool Component::IsAlive() const noexcept
{
	return m_pOwner.lock()->IsAlive();
}

const glm::mat4& Component::GetOwnerWorldTransform() const noexcept
{
	return m_pOwner.lock()->GetObjectWorldTransform();
}
