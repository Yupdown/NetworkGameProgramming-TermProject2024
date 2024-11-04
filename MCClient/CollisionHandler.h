#pragma once
#include "pch.h"

class Collider;

enum class COLLISION_TYPE
{
	COL_ENTER,
	COL_STAY,
	COL_EXIT,

	END,
};

class CollisionHandler
{
private:
	function<void(shared_ptr<Collider>&, shared_ptr<Collider>&)> m_collisionHandleFunc[etoi(COLLISION_TYPE::END)];
public:
	CollisionHandler();
	virtual ~CollisionHandler();
public:
	void SetCollisionHandlerFunc(function<void(shared_ptr<Collider>&, shared_ptr<Collider>&)> collisionFunc_,COLLISION_TYPE eType_) { m_collisionHandleFunc[etoi(eType_)] = std::move(collisionFunc_); }
	virtual void OnCollisionEnter(shared_ptr<Collider> A_,shared_ptr<Collider> B_);
	virtual void OnCollisionStay(shared_ptr<Collider> A_, shared_ptr<Collider> B_);
	virtual void OnCollisionExit(shared_ptr<Collider> A_, shared_ptr<Collider> B_);
};

