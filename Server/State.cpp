#include "pch.h"
#include "State.h"
#include "MCWorld.h"
#include "EntityMovement.h"
#include "FSM.h"
#include "Object.h"
#include "PacketBase.hpp"

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
				accTime = 0.f;
				//Protocol::s2c_MON_ATK pkt;
				//pkt.set_hit_player_id(id);
				//SessionMgr(MCWorld) << pkt;
				s2c_MON_ATK pkt;
				pkt.hit_player_id = (uint32_t)id;
				Mgr(MCWorld)->AppendToWorldSendBuffer(pkt);
			}
		}
	}
	else
	{
		accTime = 0.f;
	}
}
S_ptr<Object> FindClosestValidSession(const glm::vec3& current_position,const float th_hold)
{
	float min_distance = std::numeric_limits<float>::max();
	S_ptr<Object> closest_session = nullptr;

	const auto& players = Mgr(MCWorld)->GetWorldObjects(MC_OBJECT_TYPE::PLAYER);

	for (const auto& player : players)
	{
		const auto player_movement = player->GetEntityMovement();

		const auto entity_look = GetPlayerLook(player_movement->m_cameraAngleAxisSmooth);
		const auto dir_mon_player = glm::normalize(current_position - player_movement->current_position);
		const auto angle = glm::degrees(glm::acos(glm::dot(entity_look, dir_mon_player)));
	
		const float distance = glm::distance(current_position, player_movement->current_position);

		if (angle  <= th_hold && distance <= 30.f)
		{
			//std::cout << "!";
			if (distance < min_distance)
			{
				min_distance = distance;
				closest_session = player;
			}
		}
		
	}

	return closest_session;
}

MON_STATE Patrol::Update(EntityMovement* const move_system, const float DT)
{
	const auto entity_movement = move_system;

	const auto cur_target = FindClosestValidSession(entity_movement->current_position, 10.f);
	
	if ((cur_target == m_fsm->target) && m_fsm->target)
	{
		m_accTime += DT;
		//std::cout << "!";
		//check_and_hit(target->current_position, entity_movement->current_position, target->m_accAtkTime, DT, target->GetSessionID(),target->flag);
		if (m_accTime >= 1.5f)
		{
			entity_movement->dest = m_fsm->target->GetPos();
			entity_movement->chase_flag = true;
			return MON_STATE::CHASE;
		}
	}
	else
	{
		m_accTime = 0.f;
	}

	if(cur_target)
		m_fsm->target = cur_target;
	
	return MON_STATE::PATROL;
}

void Patrol::EnterState(const float DT)
{
}

void Patrol::ExitState(const float DT)
{
}

MON_STATE Chase::Update(EntityMovement* const move_system, const float DT)
{
	const auto entity_movement = move_system;
	auto& target = m_fsm->target;
	if (target)
	{
		if (!Mgr(MCWorld)->GetWorldObject(target->GetObjectID()))
		{
			return MON_STATE::PATROL;
		}
	}
	if (m_fsm->target)
	{
		const auto movement = target->GetEntityMovement();
		entity_movement->dest = movement->current_position;
		check_and_hit(movement->current_position, entity_movement->current_position,target->m_accAtkTime, DT, target->GetObjectID(), target->flag);
	}
	
	return MON_STATE::CHASE;
}

void Chase::EnterState(const float DT)
{
}

void Chase::ExitState(const float DT)
{
}
