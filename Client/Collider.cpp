#include "pch.h"
#include "Collider.h"
#include "Transform.h"
#include "CollisionHandler.h"
#include "GameObj.h"
#include "RigidBody.h"

GLuint Collider::g_iNextID = 0;

Collider::Collider()
	:Component{COMPONENT_TYPE::COLLIDER}
	,m_iColliderID{g_iNextID++}
	,m_pCollisionHandler{make_shared<CollisionHandler>()}
{
}

Collider::~Collider()
{
}

Collider::Collider(const Collider& other)
	:Component{COMPONENT_TYPE::COLLIDER}
	,m_iColliderID{g_iNextID++}
	,m_OBBBox{other.m_OBBBox}
	,m_pCollisionHandler{other.m_pCollisionHandler}
{
}

void Collider::Awake()
{
	m_pTransform = GetTransform();
}

shared_ptr<RigidBody> Collider::GetRigidBody() const noexcept
{
	return GetGameObj()->GetComp<RigidBody>();
}

void Collider::FinalUpdate()
{
	if (!m_bTransformDirty)
	{
		return;
	}

	m_OBBBox.transform = m_pTransform->GetLocalToWorldMatrix();
	UpdateOBB();
}

void Collider::OnCollisionEnter(shared_ptr<Collider> _pOther)
{
	++m_iCollisionCount;
	m_pCollisionHandler->OnCollisionEnter(shared_from_this(), std::move(_pOther));
}

void Collider::OnCollisionStay(shared_ptr<Collider> _pOther)
{
	m_pCollisionHandler->OnCollisionStay(shared_from_this(), std::move(_pOther));
}

void Collider::OnCollisionExit(shared_ptr<Collider> _pOther)
{
	--m_iCollisionCount;
	m_pCollisionHandler->OnCollisionExit(shared_from_this(), std::move(_pOther));
}

const bool Collider::IsDead() const
{
	return !m_pOwner.lock()->IsAlive();
}

