#include "pch.h"
#include "EntityMovement.h"
#include "AStar.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "Object.h"

void EntityMovement::Update(const float DT)
{
	// 가속도 (중력으로 초기화)
	m_vAccelation = glm::vec3(0.0f, -40.0f, 0.0f);

	// 목적지로 삼을 월드 내의 플레이어 지정
	//dest = FindTarget();

	float mul_vel = 0.f;
	const auto prev_vel = m_vAccelation;
	// 정해진 목적지가 있을 경우
	if (dest != glm::ivec3{ 0,0,0 } && chase_flag)
	{
		path = AStar::GetAStarPath(current_position, dest);

		// TODO: 만약 이전 경로와 지금 경로가 아예 같다면?
		// 꼭 그런건 아니긴 하지만, 끼어서 못움직이고 있는 경우 일 가능성이 높다.
		// 지금 끼어서 특정지점에 도달하지 못하는 반례:
		// A - B ..... C 경로인데, A -> B로 향하는 벡터로 밀어줬는데
		// 너무 멀다던지 주위 블럭이 많다던지 해서 아무리 A -> B벡터로 밀어도 B 정점에 도달 할 수 없는 경우
		// 만약 시간이 더 많다면 AStar에 "이번 턴에는 이 정점은 뽑지 마세요 리스트"를 만들어서
		// 다시 AStar돌리는 식으로 해결 하지 않을까?
		// 어쨋건, 이번 턴과 저번 턴의 경로가 같다면 그냥 그 정점을 강행돌파 하기 위해 매직넘버로 속도를 더해준다.
		if (path == prev_path) 
		{
			mul_vel = 10.f;
		}
		// 목적지까지 가는 경로가 유효한 경우
		if (path.size() >= 2)
		{
			// 가려는 위치가 한블록 위에 위치할 경우, 점프
			if (m_bGround && (path[path.size() - 2].y - path[path.size() - 1].y) >= 1)
			{
				m_vVelocity += glm::vec3(0.0f, 12.0f, 0.0f);
			}

			// 몬스터가 이동할 다음 위치를 구하기
			const glm::vec3 next_pos = glm::vec3{ path[path.size() - 2] };

			// 다음 위치로 이동하기 위해 가속도 입력
			glm::vec3 delta = next_pos - current_position;
			const auto plus_delta = delta * mul_vel;
			delta.y = 0.0f;

			if (delta.x * delta.x + delta.z * delta.z > 0.0f)
			{
				m_vAccelation += glm::normalize(delta) * m_fMoveSpeed +plus_delta;
			}
		}
	}

	// 현재 가속도를 속도에 적용
	m_vVelocity = m_vVelocity + m_vAccelation * DT;
	
	// 수평 속도의 상한선 제한
	glm::vec2 vVelocityXZ = glm::vec2(m_vVelocity.x, m_vVelocity.z);
	float l = glm::length(vVelocityXZ);
	if (l > m_vVelocityMax)
		vVelocityXZ *= m_vVelocityMax / l;
	if (l > 0.0f)
		vVelocityXZ = vVelocityXZ - glm::normalize(vVelocityXZ) * glm::min(l, 40.0f * DT);
	m_vVelocity.x = vVelocityXZ.x;
	m_vVelocity.z = vVelocityXZ.y;

	// 현재 몬스터의 위치 캐싱
	const glm::vec3 origin_pos = current_position;
	// 다음 몬스터의 위치 계산
	glm::vec3 positionPost = origin_pos + m_vVelocity * DT;

	// 타일맵과의 충돌 판정
	bool old_bGround = m_bGround;
	const auto& t = GetTileMap();
	m_bGround = t->HandleCollision(origin_pos, positionPost, m_vVelocity);

	if (current_position == positionPost)
	{
		return;
	}

	GetOwner()->SetDirtyFlag();
	// 최종 위치 적용
	current_position = positionPost;
	
	prev_path = std::move(path);
	
	m_fMoveTime += glm::length(m_vVelocity) * DT;

	const auto body_dir = glm::vec3{ dest } - current_position;
	
	m_cameraAngleAxisSmooth = body_dir;
	m_rendererBodyAngleY = glm::degrees(std::atan2(body_dir.x, body_dir.z));
}
