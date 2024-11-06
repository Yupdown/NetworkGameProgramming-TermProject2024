#include "pch.h"
#include "CollisionHandler.h"

CollisionHandler::CollisionHandler()
{
}

CollisionHandler::~CollisionHandler()
{
}

void CollisionHandler::OnCollisionEnter(shared_ptr<Collider> A_, shared_ptr<Collider> B_)
{
	if (m_collisionHandleFunc[etoi(COLLISION_TYPE::COL_ENTER)])
	{
		m_collisionHandleFunc[etoi(COLLISION_TYPE::COL_ENTER)](A_, B_);
	}
}

void CollisionHandler::OnCollisionStay(shared_ptr<Collider> A_, shared_ptr<Collider> B_)
{
	if (m_collisionHandleFunc[etoi(COLLISION_TYPE::COL_STAY)])
	{
		m_collisionHandleFunc[etoi(COLLISION_TYPE::COL_STAY)](A_, B_);
	}
}

void CollisionHandler::OnCollisionExit(shared_ptr<Collider> A_, shared_ptr<Collider> B_)
{
	if (m_collisionHandleFunc[etoi(COLLISION_TYPE::COL_EXIT)])
	{
		m_collisionHandleFunc[etoi(COLLISION_TYPE::COL_EXIT)](A_, B_);
	}
}

