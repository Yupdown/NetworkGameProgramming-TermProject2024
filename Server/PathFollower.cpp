#include "pch.h"
#include "PathFollower.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "AStar.h"
#include "Protocol.pb.h"
#include "c2s_PacketHandler.h"

glm::ivec3 dest;

// TODO:
// AStar 길만 찾는 클래스 (주기 + 플래그 + empty 여부로 길을 찾음)
//
// FSM (적어도 가장 가까운애 포인터나 좌표정돈 추적해야함)
// 충돌 (이건 필요할 수도 아닐수도)
// AStar가 FSM 이후 도는게 맞을 수도 
// .... 마지막 물리연산 하는 클래스 (지면충돌 등) 지금 이클래스가 될듯
// 최종적으로 좌표를 보내는 클래스 (별일 없다면 주기적으로 보내면 되는데 뭔가 플래그 있어서
// 바로 보낼 수도 있어야 할 듯? (RMW2 동기화 필요)

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
//    // 이동할 거리 계산
//    float move_distance = speed * deltaTime;
//    const auto& t = SINGLE(MCWorld)->GetTileMap();
//
//    // 속도 증가 비율 (점프할 때 속도 증가)
//    float speed_multiplier = 1.0f;
//
//    // 중간 경유지를 기억하기 위한 변수
//    static bool moving_to_midpoint = false;
//    static glm::vec3 midpoint;
//
//    glm::vec3 last_dir = {};
//    // 경로 상의 마지막 포인트부터 이동
//    while (!path.empty() && move_distance > 0.0f) {
//        glm::ivec3 next_point = path.back();
//        glm::vec3 next_point_world = glm::vec3(next_point.x + 0.f, next_point.y, next_point.z + 0.f) / 10.0f;
//
//        // 단차 확인: y축 차이가 1 이상인 경우 중간 경유지 설정
//        if (std::abs(next_point_world.y - current_position.y) >= .5f && !moving_to_midpoint) {
//            // 경유지 설정: y 값이 더 높으면 약간 더 높게, 낮으면 약간 더 낮게
//            if (next_point_world.y > current_position.y) {
//                midpoint = glm::vec3(current_position.x, next_point_world.y + 10.f, current_position.z);  // 살짝 위로
//            }
//            else {
//                midpoint = glm::vec3(current_position.x, next_point_world.y + 5.f, current_position.z);  // 살짝 아래로
//            }
//            moving_to_midpoint = true;
//
//            // 속도를 일시적으로 증가시킴 (점프 효과)
//            speed_multiplier = 1.5f;  // 속도 50% 증가
//        }
//
//        // 이동할 위치를 결정 (중간 경유지 또는 다음 경로)
//        glm::vec3 target_position = moving_to_midpoint ? midpoint : next_point_world;
//
//        glm::vec3 direction = glm::normalize(target_position - current_position);
//        float distance_to_target = glm::distance(current_position, target_position);
//
//        last_dir = direction;
//
//        if (move_distance >= distance_to_target) {
//            // 중간 지점 또는 다음 포인트로 이동 가능한 경우
//            const auto pre = current_position;
//            current_position = target_position;
//            auto v = direction * speed * speed_multiplier;
//            //t->HandleCollision(pre, current_position, v);
//            move_distance -= distance_to_target;
//
//            // 중간 경유지에 도착한 경우
//            if (moving_to_midpoint) {
//                moving_to_midpoint = false;  // 중간 경유지를 해제
//                speed_multiplier = 1.0f;     // 속도를 원래대로 복귀
//            }
//            else {
//                // 다음 포인트로 이동 가능한 경우
//                path.pop_back();  // 다음 포인트를 경로에서 제거
//            }
//        }
//        else {
//            // 이동할 거리가 남은 경우 중간 지점까지 또는 포인트까지 부분 이동
//            const auto pre = current_position;
//            current_position += direction * move_distance * speed_multiplier;
//            auto v = direction * speed * speed_multiplier;
//            // t->HandleCollision(pre, current_position, v);
//            move_distance = 0.0f;
//
//            // 속도를 원래대로 복귀 (경유지를 벗어난 경우)
//            if (!moving_to_midpoint) {
//                speed_multiplier = 1.0f;
//            }
//        }
//    }
//    std::cout << current_position.x << ' ' << current_position.y << ' ' << current_position.z << '\n';
//}