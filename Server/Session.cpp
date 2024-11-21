#include "pch.h"
#include "Session.h"
#include "Object.h"
#include "Component.h"

Session::Session(const uint64_t id, const SOCKET sock)noexcept
	: m_sessionID{ id }
	, m_socket{ sock }
{
}

Session::~Session() noexcept
{
	std::cout << "~Session\n";
	::closesocket(m_socket);
}

void Session::SetObjectInvalid() noexcept
{
	if (m_myGameObject)
	{
		m_myGameObject->SetInvalid();
		m_myGameObject.reset();
	}
}
