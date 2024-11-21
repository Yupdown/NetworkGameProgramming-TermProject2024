#pragma once
#include "pch.h"

class Component;
class Session;
class SendBuffer;
class EntityMovement;

class Object
{
public:
	Object();
	Object(std::shared_ptr<Session> session);
public:
	const auto& GetSession()const noexcept { return m_session; }
	const auto GetObjectID()const noexcept { return (uint32_t)m_obj_id; }
	const auto GetObjectType()const noexcept { return m_eObjType; }
	const auto GetEntityMovement()const noexcept { return m_pEntityMovemet; }
	void SetPos(const glm::vec3& pos)noexcept { m_pos = pos; }
	const auto& GetPos()const noexcept { return m_pos; }
public:
	void Init()noexcept;
	void Update(const float DT)noexcept;
	void SetDirtyFlag()noexcept { m_bDirtyFlag = true; }
	const auto& AddComp(Component* const pComp)noexcept { return m_vecComp.emplace_back(pComp); }
	void SetObjectType(const MC_OBJECT_TYPE eType) { m_eObjType = eType; }
private:
	glm::vec3 m_pos{};
	MC_OBJECT_TYPE m_eObjType;
	const uint64_t m_obj_id;
	const std::shared_ptr<Session> m_session;
	std::vector<std::unique_ptr<Component>> m_vecComp;
	EntityMovement* const m_pEntityMovemet;
	bool m_bDirtyFlag = false;
};

