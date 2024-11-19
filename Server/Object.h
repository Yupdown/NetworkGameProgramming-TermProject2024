#pragma once
#include "pch.h"

class Component;
class Session;
class SendBuffer;

class Object
{
public:
	Object();
	Object(std::shared_ptr<Session> session);
public:
	const auto& GetSession()const noexcept { return m_session; }
	const auto GetObjectID()const noexcept { return m_obj_id; }
public:
	void Update(const float DT)noexcept;
private:
	const uint64_t m_obj_id;
	const std::shared_ptr<Session> m_session;
	std::vector<std::unique_ptr<Component>> m_mapComp;
};

