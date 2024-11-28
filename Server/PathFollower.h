#pragma once
#include "Component.h"

class PathFollower
    :public Component
{
public:
    void Update(const float DT)override;
    
public:
    std::vector<glm::ivec3> path;
    std::vector<glm::ivec3> prev_path;
    float m_fMoveSpeed = 100.0f;
    glm::ivec3 dest;
    float m_vVelocityMax = 5.5f;
    bool m_bGround = false;
    float m_fMoveTime = 0.0f;
    bool chase_flag = false;
};

