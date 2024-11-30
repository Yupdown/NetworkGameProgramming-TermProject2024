#include "pch.h"
#include "EnderDragon.h"
#include "MCTilemap.h"
#include "Object.h"

void EnderDragon::Init()
{
    constexpr const float BASE_X = MCTilemap::MAP_WIDTH / 2;
    constexpr const float BASE_Y = MCTilemap::MAP_HEIGHT - 10;
    constexpr const float BASE_Z = MCTilemap::MAP_WIDTH / 2;

    for (int i = 0; i < 20; ++i)
    {
        const float x = BASE_X + static_cast<float>(std::rand()) / RAND_MAX * 120.0f - 60.0f; 
        const float y = BASE_Y + static_cast<float>(std::rand()) / RAND_MAX * 10.0f - 5.0f; 
        const float z = BASE_Z + static_cast<float>(std::rand()) / RAND_MAX * 120.0f - 60.0f;
        m_randomPoints.emplace_back(x, y, z);
    }

   const auto owner = GetOwner();

   p0 = owner->GetPos();
   p1 = m_randomPoints[std::rand() % m_randomPoints.size()];
   p2 = m_randomPoints[std::rand() % m_randomPoints.size()];

   owner->SetPos(bezier(p0, p1, p2, m_bezierT));
}

void EnderDragon::Update(const float DT)
{
    m_bezierT = glm::fclamp(m_bezierT += DT / 10.f, 0.f, 1.f);
    const auto owner = GetOwner();
    owner->SetDirtyFlag();
    if (1.f <= m_bezierT)
    {
        m_bezierT = 0.f;

        p0 = owner->GetPos();
        p1 = m_randomPoints[std::rand() % m_randomPoints.size()];
        p2 = m_randomPoints[std::rand() % m_randomPoints.size()];

        owner->SetPos(bezier(p0, p1, p2, m_bezierT));
    }
    else
    {
        owner->SetPos(bezier(p0, p1, p2, m_bezierT));
    }
}
