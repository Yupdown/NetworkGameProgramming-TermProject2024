#include "pch.h"
#include "Projectile.h"
#include "Object.h"
#include "MCTilemap.h"
#include "MCWorld.h"
#include "PacketBase.hpp"

glm::vec3 TransformPointToCylinderLocal(
    const glm::vec3& point,
    const glm::vec3& cylinderBase,
    const float zRotation,
    const float yRotation
)noexcept
{
    const glm::vec3 translatedPoint = point - cylinderBase;
    const glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), yRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::vec4 rotatedPointY = rotationY * glm::vec4(translatedPoint, 1.0f);
    const glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), zRotation, glm::vec3(0.0f, 0.0f, 1.0f));
    const glm::vec4 rotatedPointZ = rotationZ * rotatedPointY;

    return rotatedPointZ;
}

void Projectile::CheckGroupCollision(Projectile* const proj, const MC_OBJECT_TYPE type, const float radius, const float height, const int dmg) noexcept
{
    const auto& target_group = Mgr(MCWorld)->GetWorldObjects(type);
    const auto owner = proj->GetOwner();
    const auto project_pos = owner->GetPos();
    const float collision_radius = radius;
    const float collision_height = height;

    for (const auto& target : target_group)
    {
        const auto& target_pos = target->GetPos();
        const bool is_hit = glm::distance(glm::vec2(target_pos.x, target_pos.z), glm::vec2(project_pos.x, project_pos.z)) <= collision_radius
            && project_pos.y >= target_pos.y && project_pos.y <= target_pos.y + collision_height;
        if (is_hit)
        {
            //  std::cout << "Ãæµ¹!\n";
            target->DecHP(dmg);
            owner->SetInvalid();
            return;
        }
    }
}

void Projectile::Update(const float DT)
{
    if (!m_isPinned)
    {
        UpdateProjectile(DT);
    }
    else
    {
        m_accProjTime += DT;
        if (G_PROJ_REMAIN_TIME <= m_accProjTime)
        {
            GetOwner()->SetInvalid();
        }
    }
}

void ProjArrow::CheckCollisionToEnderDragon()
{
    const auto& bs = Mgr(MCWorld)->GetWorldObjects(MC_OBJECT_TYPE::BOSS);
    const auto num = bs.size();
    if (0 == num)return;
    auto b = bs.data();
    const auto owner = GetOwner();
    const auto e = b + num;
    constexpr const float BOSS_RADIUS = 2.f;
    constexpr const float BOSS_HEIGHT = 7.f;
    const auto project_pos = owner->GetPos();
    while (e != b)
    {
        const auto& ed = (*b++);
        const auto& pos_info = ed->GetPosInfo();

        const auto& monster_pos = pos_info.m_vPos;
        const float collision_radius = BOSS_RADIUS;
        const float collision_height = BOSS_HEIGHT;

        const auto v = ed->GetVelocity();
        const auto r = glm::degrees(-std::atan2(v.z, v.x)) - 90.f;

        const glm::vec3 localPoint = TransformPointToCylinderLocal(project_pos, monster_pos, glm::radians(90.f), glm::radians(r));

        const float distanceFromCenter = std::sqrt(localPoint.x * localPoint.x + localPoint.z * localPoint.z);
        const bool isWithinRadius = distanceFromCenter <= BOSS_RADIUS;
        const bool isWithinHeight = localPoint.y >= 0 && localPoint.y <= BOSS_HEIGHT;

        const bool is_hit = isWithinRadius && isWithinHeight;

        if (is_hit)
        {
            ed->DecHP(G_ARROW_DMG);
            owner->SetInvalid();
            return;
        }

    }
}

void ProjArrow::UpdateProjectile(const float DT) noexcept
{
    const auto& tile_map = Mgr(MCWorld)->GetTileMap();
    const auto owner = GetOwner();
    const auto pos = owner->GetPos();

    float gravity = 9.8f * 2.0f;
    glm::vec3 pos_pre = pos;
    glm::vec3 v = owner->GetVelocity();
    v.y -= gravity * DT;
    glm::vec3 pos_post = pos_pre + v * DT;

    glm::ivec3 tilePos = pos_post;

    if (tile_map->GetTileFlag(tilePos.x, tilePos.y, tilePos.z) != 0)
    {
        m_isPinned = true;
        v = glm::zero<glm::vec3>();
    }

    owner->SetPos(pos_post);
    owner->SetVelocity(v);

    if (m_isPinned)return;

    Projectile::CheckGroupCollision(this, MC_OBJECT_TYPE::MONSTER, 0.5f, 3.0f, G_ARROW_DMG);
    Projectile::CheckGroupCollision(this, MC_OBJECT_TYPE::PLAYER, 0.5f, 2.0f, G_ARROW_DMG);
    CheckCollisionToEnderDragon();
}

void ProjFireBall::UpdateProjectile(const float DT) noexcept
{
    const auto& tile_map = Mgr(MCWorld)->GetTileMap();
    const auto owner = GetOwner();
    auto& pos_info = owner->GetPosInfo();

    const glm::vec3 pos_pre = pos_info.m_vPos;
    const glm::vec3 v = pos_info.m_vVelocity;
    const glm::vec3 pos_post = pos_pre + v * DT;
    const glm::ivec3 tilePos = pos_post;

    if (tile_map->GetTile(tilePos) != 0 && !m_isPinned)
    {
        pos_info.m_vVelocity = glm::zero<glm::vec3>();
        m_isPinned = true;
    }

    pos_info.m_vPos = pos_post;
    if (m_isPinned)return;

    Projectile::CheckGroupCollision(this, MC_OBJECT_TYPE::PLAYER, 1.f, 3.0f, G_FIRE_BALL_DMG);
}
