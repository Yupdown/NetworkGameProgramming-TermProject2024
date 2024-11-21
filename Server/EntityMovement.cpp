#include "pch.h"
#include "EntityMovement.h"
#include "AStar.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "Object.h"

void EntityMovement::Update(const float DT)
{
	// ���ӵ� (�߷����� �ʱ�ȭ)
	m_vAccelation = glm::vec3(0.0f, -40.0f, 0.0f);

	// �������� ���� ���� ���� �÷��̾� ����
	//dest = FindTarget();

	float mul_vel = 0.f;
	const auto prev_vel = m_vAccelation;
	// ������ �������� ���� ���
	if (dest != glm::ivec3{ 0,0,0 } && chase_flag)
	{
		path = AStar::GetAStarPath(current_position, dest);

		// TODO: ���� ���� ��ο� ���� ��ΰ� �ƿ� ���ٸ�?
		// �� �׷��� �ƴϱ� ������, ��� �������̰� �ִ� ��� �� ���ɼ��� ����.
		// ���� ��� Ư�������� �������� ���ϴ� �ݷ�:
		// A - B ..... C ����ε�, A -> B�� ���ϴ� ���ͷ� �о���µ�
		// �ʹ� �ִٴ��� ���� ���� ���ٴ��� �ؼ� �ƹ��� A -> B���ͷ� �о B ������ ���� �� �� ���� ���
		// ���� �ð��� �� ���ٸ� AStar�� "�̹� �Ͽ��� �� ������ ���� ������ ����Ʈ"�� ����
		// �ٽ� AStar������ ������ �ذ� ���� ������?
		// ��¶��, �̹� �ϰ� ���� ���� ��ΰ� ���ٸ� �׳� �� ������ ���൹�� �ϱ� ���� �����ѹ��� �ӵ��� �����ش�.
		if (path == prev_path) 
		{
			mul_vel = 10.f;
		}
		// ���������� ���� ��ΰ� ��ȿ�� ���
		if (path.size() >= 2)
		{
			// ������ ��ġ�� �Ѻ�� ���� ��ġ�� ���, ����
			if (m_bGround && (path[path.size() - 2].y - path[path.size() - 1].y) >= 1)
			{
				m_vVelocity += glm::vec3(0.0f, 12.0f, 0.0f);
			}

			// ���Ͱ� �̵��� ���� ��ġ�� ���ϱ�
			const glm::vec3 next_pos = glm::vec3{ path[path.size() - 2] };

			// ���� ��ġ�� �̵��ϱ� ���� ���ӵ� �Է�
			glm::vec3 delta = next_pos - current_position;
			const auto plus_delta = delta * mul_vel;
			delta.y = 0.0f;

			if (delta.x * delta.x + delta.z * delta.z > 0.0f)
			{
				m_vAccelation += glm::normalize(delta) * m_fMoveSpeed +plus_delta;
			}
		}
	}

	// ���� ���ӵ��� �ӵ��� ����
	m_vVelocity = m_vVelocity + m_vAccelation * DT;
	
	// ���� �ӵ��� ���Ѽ� ����
	glm::vec2 vVelocityXZ = glm::vec2(m_vVelocity.x, m_vVelocity.z);
	float l = glm::length(vVelocityXZ);
	if (l > m_vVelocityMax)
		vVelocityXZ *= m_vVelocityMax / l;
	if (l > 0.0f)
		vVelocityXZ = vVelocityXZ - glm::normalize(vVelocityXZ) * glm::min(l, 40.0f * DT);
	m_vVelocity.x = vVelocityXZ.x;
	m_vVelocity.z = vVelocityXZ.y;

	// ���� ������ ��ġ ĳ��
	const glm::vec3 origin_pos = current_position;
	// ���� ������ ��ġ ���
	glm::vec3 positionPost = origin_pos + m_vVelocity * DT;

	// Ÿ�ϸʰ��� �浹 ����
	bool old_bGround = m_bGround;
	const auto& t = GetTileMap();
	m_bGround = t->HandleCollision(origin_pos, positionPost, m_vVelocity);

	if (current_position == positionPost)
	{
		return;
	}

	GetOwner()->SetDirtyFlag();
	// ���� ��ġ ����
	current_position = positionPost;
	
	prev_path = std::move(path);
	
	m_fMoveTime += glm::length(m_vVelocity) * DT;

	const auto body_dir = glm::vec3{ dest } - current_position;
	
	m_cameraAngleAxisSmooth = body_dir;
	m_rendererBodyAngleY = glm::degrees(std::atan2(body_dir.x, body_dir.z));
}
