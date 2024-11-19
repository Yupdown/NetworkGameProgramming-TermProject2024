#include "pch.h"
#include "Object.h"
#include "Session.h"
#include "IOExecutor.h"

Object::Object()
	: m_obj_id{ IssueGlobalObjectID() }
	, m_session{}
{
}

Object::Object(std::shared_ptr<Session> session)
	: m_obj_id{ session->GetSessionID()}
	, m_session{ std::move(session) }
{
}
