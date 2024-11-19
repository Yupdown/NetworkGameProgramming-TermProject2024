#pragma once
#include "Component.h"

class EntityMovement
    :public Component
{
public:
    void Update(const float DT)override;
    
public:
    std::vector<glm::ivec3> path;
    std::vector<glm::ivec3> prev_path;
    float m_fMoveSpeed = 100.0f;
    glm::vec3 current_position;
    glm::vec3 m_vVelocity = glm::zero<glm::vec3>();
    glm::vec3 m_vAccelation = glm::zero<glm::vec3>();
    glm::ivec3 dest;
    float m_vVelocityMax = 5.5f;
    bool m_bGround = false;






    glm::vec3 m_cameraAngleAxis = glm::zero<glm::vec3>();
    glm::vec3 m_cameraAngleAxisSmooth = glm::zero<glm::vec3>();
    float m_fMoveTime = 0.0f;
    float m_rendererBodyAngleY = 0.f;

    bool chase_flag = false;
};

