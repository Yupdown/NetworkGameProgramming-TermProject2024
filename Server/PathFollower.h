#pragma once
#include "Component.h"

class PathFollower
    :public Component
{
private:
    static void ClearPath()noexcept {
        path.clear();
        prev_path.clear();
    }
public:
    void Update(const float DT)override;
    
public:
    constinit static inline std::vector<glm::ivec3> path;
    constinit static inline std::vector<glm::ivec3> prev_path;
    static constexpr const float m_vVelocityMax = 5.5f;

    float m_fMoveSpeed = 100.0f;
    glm::ivec3 dest;
   
    bool m_bGround = false;
    float m_fMoveTime = 0.0f;
    bool chase_flag = false;
};

