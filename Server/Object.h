#pragma once
#include "pch.h"

class Component;
class Session;

class Object
{
public:
	Object();
	Object(std::shared_ptr<Session> session);
public:
	const auto& GetSession()const noexcept { return m_session; }
private:
	const std::shared_ptr<Session> m_session;
	std::vector<std::unique_ptr<Component>> m_mapComp;
};

