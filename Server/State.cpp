#include "pch.h"
#include "State.h"
#include "MCWorld.h"
#include "PathFollower.h"
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
				
				const auto player = Mgr(MCWorld)->GetWorldObject(id);
				player->DecHP(1);
			}
		}
	}
	else
	{
		// 이거 하면 X
		// accTime = 0.f;
	}
}
S_ptr<Object> FindClosestValidSession(const glm::vec3& current_position,const float th_hold)
{
	float min_distance = std::numeric_limits<float>::max();
	S_ptr<Object> closest_session = nullptr;

	const auto& players = Mgr(MCWorld)->GetWorldObjects(MC_OBJECT_TYPE::PLAYER);

	for (const auto& player : players)
	{
		const auto player_movement = &player->GetPosInfo();

		const auto entity_look = GetPlayerLook(player_movement->m_cameraAngleAxisSmooth);
		const auto dir_mon_player = glm::normalize(current_position - player_movement->m_vPos);
		const auto angle = glm::degrees(glm::acos(glm::dot(entity_look, dir_mon_player)));
	
		const float distance = glm::distance(current_position, player_movement->m_vPos);

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

MON_STATE Patrol::Update(PositionInfo* const pos_info, const float DT)
{
	const auto pf = m_fsm->GetPathFollower();
	const auto cur_target = FindClosestValidSession(pos_info->m_vPos, 10.f);
	
	if ((cur_target == m_fsm->target) && m_fsm->target)
	{
		m_accTime += DT;
		//std::cout << "!";
		//check_and_hit(target->current_position, entity_movement->current_position, target->m_accAtkTime, DT, target->GetSessionID(),target->flag);
		if (m_accTime >= 1.5f)
		{
			// return MON_STATE::PATROL; 충돌검사 테스트용으로 하려면 이렇게
			pf->dest = m_fsm->target->GetPos();
			pf->chase_flag = true;
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

MON_STATE Chase::Update(PositionInfo* const pos_info, const float DT)
{
	auto& target = m_fsm->target;
	if (target)
	{
		if (!Mgr(MCWorld)->GetWorldObject(target->GetObjectID()))
		{
			target.reset();
			return MON_STATE::PATROL;
		}
		const auto movement = &target->GetPosInfo();
		m_fsm->GetPathFollower()->dest = movement->m_vPos;
		check_and_hit(movement->m_vPos, pos_info->m_vPos, target->m_accAtkTime, DT, target->GetObjectID(), target->flag);
	}
	
	return MON_STATE::CHASE;
}

void Chase::EnterState(const float DT)
{
}

void Chase::ExitState(const float DT)
{
}
