#include "pch.h"
#include "State.h"
#include "MCWorld.h"
#include "EntityMovement.h"
#include "FSM.h"
#include "Object.h"


glm::vec3 GetPlayerLook(const glm::vec3& angle_axis) noexcept
{
	return glm::rotate(glm::quat(glm::vec3(glm::radians(angle_axis.x), glm::radians(angle_axis.y), 0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
}

void check_and_hit(const glm::vec3& a, const glm::vec3& b,float& accTime,const float DT,uint64_t id,bool& flag)
{
	// std::cout << glm::distance(a, b) << std::endl;
	if (glm::distance(a, b) <= ATK_DIST)
	{
		if (flag)
		{
			flag = false;
			accTime += DT;
			if (accTime >= ATK_TIME)
			{
				//accTime = 0.f;
				//Protocol::s2c_MON_ATK pkt;
				//pkt.set_hit_player_id(id);
				//SessionMgr(MCWorld) << pkt;
			}
		}
	}
	else
	{
		//accTime = 0.f;
	}
}
S_ptr<class ClientSession >FindClosestValidSession(const glm::vec3& current_position,const float th_hold)
{
	float min_distance = std::numeric_limits<float>::max();
	S_ptr<class ClientSession> closest_session = nullptr;

	//for (const auto& a_session : SINGLE(MCWorld)->m_arrSessions)
	//{
	//	const auto session = a_session.load();
	//	if (!session || !session->ready) continue;
	//	const auto entity_look = GetPlayerLook(session->m_cameraAngleAxisSmooth);
	//	const auto dir_mon_player = glm::normalize(current_position - session->current_position);
	//	const auto angle = glm::degrees(glm::acos(glm::dot(entity_look, dir_mon_player)));
	//
	//	const float distance = glm::distance(current_position, session->current_position); 
	//	if (angle  <= th_hold && distance <= 30.f)
	//	{
	//		
	//		if (distance < min_distance)
	//		{
	//			min_distance = distance;
	//			closest_session = session;
	//		}
	//	}
	//}

	return closest_session;
}

MON_STATE Patrol::Update(const float DT)
{
	const auto entity_movement = m_fsm->GetOwner()->GetComp<EntityMovement>();

	//const auto cur_target = FindClosestValidSession(entity_movement->current_position, 10.f);
	//
	//if ((cur_target == m_fsm->target) && m_fsm->target)
	//{
	//	m_accTime += DT;
	//	//check_and_hit(target->current_position, entity_movement->current_position, target->m_accAtkTime, DT, target->GetSessionID(),target->flag);
	//	if (m_accTime >= 1.5f)
	//	{
	//		entity_movement->dest = m_fsm->target->GetPos();
	//		entity_movement->chase_flag = true;
	//		return MON_STATE::CHASE;
	//	}
	//}
	//else
	//{
	//	m_accTime = 0.f;
	//}

	//if(cur_target)
	//	m_fsm->target = cur_target;
	
	return MON_STATE::PATROL;
}

void Patrol::EnterState(const float DT)
{
}

void Patrol::ExitState(const float DT)
{
}

MON_STATE Chase::Update(const float DT)
{
	//const auto entity_movement = m_fsm->m_pOwner->GetComp<EntityMovement>();
	//auto& target = m_fsm->target;
	//if (target)
	//{
	//	if (const auto p = SINGLE(MCWorld)->m_arrSessions[target->my_count].load())
	//	{
	//		if (target->GetSessionID() != p->GetSessionID())
	//		{
	//			target = nullptr;
	//			return MON_STATE::PATROL;
	//		}
	//	}
	//}
	//if (m_fsm->target)
	//{
	//	
	//	entity_movement->dest = target->current_position;
	//	check_and_hit(target->current_position, entity_movement->current_position, target->m_accAtkTime, DT, target->GetSessionID(), target->flag);
	//}
	//
	return MON_STATE::CHASE;
}

void Chase::EnterState(const float DT)
{
}

void Chase::ExitState(const float DT)
{
}
