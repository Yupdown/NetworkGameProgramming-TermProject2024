#include "pch.h"
#include "PathFollower.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "AStar.h"
#include "Protocol.pb.h"
#include "c2s_PacketHandler.h"

glm::ivec3 dest;

// TODO:
// AStar �游 ã�� Ŭ���� (�ֱ� + �÷��� + empty ���η� ���� ã��)
//
// FSM (��� ���� ������ �����ͳ� ��ǥ���� �����ؾ���)
// �浹 (�̰� �ʿ��� ���� �ƴҼ���)
// AStar�� FSM ���� ���°� ���� ���� 
// .... ������ �������� �ϴ� Ŭ���� (�����浹 ��) ���� ��Ŭ������ �ɵ�
// ���������� ��ǥ�� ������ Ŭ���� (���� ���ٸ� �ֱ������� ������ �Ǵµ� ���� �÷��� �־
// �ٷ� ���� ���� �־�� �� ��? (RMW2 ����ȭ �ʿ�)

void PathFollower::Update(const float DT)
{
	if (dest == glm::ivec3{ 0,0,0 })return;
    path = AStar::GetAStarPath(current_position*10.f, dest);
    if (path.empty()) return;
	if (path.size() <= 2)return;
	//current_position = path[path.size() - 2];
	//current_position /= 10.f;
	//std::cout << path[path.size() - 2].x << ' ' << path[path.size() - 2].y << ' ' << path[path.size() - 2].z << '\n';
	const glm::vec3 next_pos = glm::vec3{ path[path.size() - 2] } / 10.f;
	//if(m_bGround)
	
	m_vAccelation = glm::vec3(.0f, -4.f, 0.0f);
	if ((path[path.size() - 2].y - path[path.size() - 1].y) >= 1) {
		m_vAccelation = glm::vec3(.0f, 1.f, 0.0f);
		//m_vVelocity.y += 2.f;
	}
	const auto dir = glm::normalize(next_pos - current_position) * speed;
	const auto& t = SINGLE(MCWorld)->GetTileMap();
	m_vAccelation += dir * speed * DT;

	m_vVelocity = m_vVelocity + m_vAccelation * DT;
	//std::cout << m_vVelocity.x << ' ' << m_vVelocity.y << ' ' << m_vVelocity.z << '\n';
	glm::vec2 vVelocityXZ = glm::vec2(m_vVelocity.x, m_vVelocity.z);
	float l = glm::length(vVelocityXZ);
	if (l > m_vVelocityMax)
		vVelocityXZ *= m_vVelocityMax / l;
	if (l > 0.0f)
		vVelocityXZ = vVelocityXZ - glm::normalize(vVelocityXZ) * glm::min(l, 4.0f * DT);
	m_vVelocity.x = vVelocityXZ.x;
	m_vVelocity.z = vVelocityXZ.y;

	const glm::vec3 origin_pos = current_position;

	const glm::vec3 position = origin_pos * 10.0f;
	glm::vec3 positionPost = position + m_vVelocity * DT * speed;

	bool old_bGround = m_bGround;
	m_bGround = t->HandleCollision(position, positionPost, m_vVelocity);
	current_position = positionPost * 0.1f;
	
	float old_fMoveTime = m_fMoveTime;
	m_fMoveTime += glm::length(m_vVelocity) * DT;

    Protocol::s2c_MOVE pkt;
	//current_position.y =( dest.y / 10);
	pkt.set_playerid(100);
	const auto mvpos = pkt.mutable_othermovedata()->mutable_vposition();
	mvpos->set_x(current_position.x);
	mvpos->set_y(current_position.y);
	mvpos->set_z(current_position.z);

	const auto mvaccel = pkt.mutable_othermovedata()->mutable_vaccelation();
	mvaccel->set_x(m_vAccelation.x);
	mvaccel->set_y(m_vAccelation.y);
	mvaccel->set_z(m_vAccelation.z);

	const auto mvel = pkt.mutable_othermovedata()->mutable_vvelocity();
	mvel->set_x(m_vVelocity.x);
	mvel->set_y(m_vVelocity.y);
	mvel->set_z(m_vVelocity.z);

	//std::cout << current_position.x << ' '<<current_position.y << ' '<<current_position.z << '\n';
    SessionMgr(MCWorld) << pkt;
}
//void PathFollower::Update(float deltaTime)
//{
//    if (path.empty()) return;
//
//    // �̵��� �Ÿ� ���
//    float move_distance = speed * deltaTime;
//    const auto& t = SINGLE(MCWorld)->GetTileMap();
//
//    // �ӵ� ���� ���� (������ �� �ӵ� ����)
//    float speed_multiplier = 1.0f;
//
//    // �߰� �������� ����ϱ� ���� ����
//    static bool moving_to_midpoint = false;
//    static glm::vec3 midpoint;
//
//    glm::vec3 last_dir = {};
//    // ��� ���� ������ ����Ʈ���� �̵�
//    while (!path.empty() && move_distance > 0.0f) {
//        glm::ivec3 next_point = path.back();
//        glm::vec3 next_point_world = glm::vec3(next_point.x + 0.f, next_point.y, next_point.z + 0.f) / 10.0f;
//
//        // ���� Ȯ��: y�� ���̰� 1 �̻��� ��� �߰� ������ ����
//        if (std::abs(next_point_world.y - current_position.y) >= .5f && !moving_to_midpoint) {
//            // ������ ����: y ���� �� ������ �ణ �� ����, ������ �ణ �� ����
//            if (next_point_world.y > current_position.y) {
//                midpoint = glm::vec3(current_position.x, next_point_world.y + 10.f, current_position.z);  // ��¦ ����
//            }
//            else {
//                midpoint = glm::vec3(current_position.x, next_point_world.y + 5.f, current_position.z);  // ��¦ �Ʒ���
//            }
//            moving_to_midpoint = true;
//
//            // �ӵ��� �Ͻ������� ������Ŵ (���� ȿ��)
//            speed_multiplier = 1.5f;  // �ӵ� 50% ����
//        }
//
//        // �̵��� ��ġ�� ���� (�߰� ������ �Ǵ� ���� ���)
//        glm::vec3 target_position = moving_to_midpoint ? midpoint : next_point_world;
//
//        glm::vec3 direction = glm::normalize(target_position - current_position);
//        float distance_to_target = glm::distance(current_position, target_position);
//
//        last_dir = direction;
//
//        if (move_distance >= distance_to_target) {
//            // �߰� ���� �Ǵ� ���� ����Ʈ�� �̵� ������ ���
//            const auto pre = current_position;
//            current_position = target_position;
//            auto v = direction * speed * speed_multiplier;
//            //t->HandleCollision(pre, current_position, v);
//            move_distance -= distance_to_target;
//
//            // �߰� �������� ������ ���
//            if (moving_to_midpoint) {
//                moving_to_midpoint = false;  // �߰� �������� ����
//                speed_multiplier = 1.0f;     // �ӵ��� ������� ����
//            }
//            else {
//                // ���� ����Ʈ�� �̵� ������ ���
//                path.pop_back();  // ���� ����Ʈ�� ��ο��� ����
//            }
//        }
//        else {
//            // �̵��� �Ÿ��� ���� ��� �߰� �������� �Ǵ� ����Ʈ���� �κ� �̵�
//            const auto pre = current_position;
//            current_position += direction * move_distance * speed_multiplier;
//            auto v = direction * speed * speed_multiplier;
//            // t->HandleCollision(pre, current_position, v);
//            move_distance = 0.0f;
//
//            // �ӵ��� ������� ���� (�������� ��� ���)
//            if (!moving_to_midpoint) {
//                speed_multiplier = 1.0f;
//            }
//        }
//    }
//    std::cout << current_position.x << ' ' << current_position.y << ' ' << current_position.z << '\n';
//}