#pragma once
#include "pch.h"
#include "Component.h"

class PathFollower 
    :public Component
{
public:
    PathFollower() = default;
    PathFollower(glm::vec3 v):current_position{v}{}
    void SetPath(const std::vector<glm::ivec3>& new_path) {
        path = new_path;
        if (!path.empty()) {
            // 경로의 마지막 정수 좌표를 월드 좌표로 변환하여 current_position에 저장
           current_position = glm::vec3(path.back().x + 0.f, path.back().y, path.back().z + 0.f) / 10.0f;
        }
    }

    void SetSpeed(float new_speed) {
        speed = new_speed;
    }

    void Update(const float deltaTime) override;

    glm::ivec3 GetCurrentPosition() const {
        return current_position;
    }

public:
    std::vector<glm::ivec3> path;
    float speed = 10.0f;
    glm::vec3 current_position;
    glm::vec3 m_vVelocity = glm::zero<glm::vec3>();
    glm::vec3 m_vAccelation = glm::zero<glm::vec3>();
    float m_vVelocityMax = 0.65f;
    bool m_bGround = false;
    float m_fMoveSpeed = 10.f;
    float m_fMoveTime = 0.0f;
};