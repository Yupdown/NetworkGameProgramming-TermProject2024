#include "pch.h"
#include "ProjArrow.h"
#include "Object.h"
#include "MCTilemap.h"
#include "MCWorld.h"
#include "PacketBase.hpp"

void ProjArrow::Update(const float DT)
{
    
    if (!m_isPinned)
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

        if (tile_map->GetTileFlag(tilePos.x,tilePos.y,tilePos.z) != 0)
        {
            m_isPinned = true;
            v = glm::zero<glm::vec3>();
        }

        owner->SetPos(pos_post);
        owner->SetVelocity(v);
       
        // �� �׷쿡 ���� �浹 �˻縦 ������ ���� �Լ�
        constexpr const auto group_collision_func = [](ProjArrow* const pArrow, MC_OBJECT_TYPE type, float radius, float height)noexcept {
            const auto& monster_group = Mgr(MCWorld)->GetWorldObjects(type);
            const auto owner = pArrow->GetOwner();
            const auto project_pos = owner->GetPos();
            for (const auto& monster : monster_group)
            {
                const auto& monster_pos = monster->GetPos();
                const float collision_radius = radius;
                const float collision_height = height;

                // ȭ�� �浹 ���� ��� (�� �� ����� �浹)
                bool is_hit = true;
                is_hit &= glm::distance(glm::vec2(monster_pos.x, monster_pos.z), glm::vec2(project_pos.x, project_pos.z)) <= collision_radius;
                is_hit &= project_pos.y >= monster_pos.y && project_pos.y <= monster_pos.y + collision_height;
                if (is_hit)
                {
                    // ������Ʈ���� ������� ����
                    // ���Ͱ� Ȯ���ϴ� �ϰ� static
                    //const auto serverObj = static_cast<ServerObject*>(monster.get());
                    //serverObj->OnObjectDamaged(ARROW_DMG);

                    // ȭ�� ����; ����������Ʈ�� ��ȯ�� ServerObjectManager::RemoveObject() ȣ��
                    //DestroyObj(std::move(pArrow));
                    std::cout << "�浹!\n";
                    monster->DecHP(G_ARROW_DMG);
                    owner->SetInvalid();
                    return;
                }
            }
            };
        if (m_isPinned)return;
        // TODO: �и��ϰ� ��üȭ
        group_collision_func(this, MC_OBJECT_TYPE::MONSTER, 0.5f, 3.0f);
        group_collision_func(this, MC_OBJECT_TYPE::PLAYER, 0.5f, 2.0f); 
    }
    // ȭ���� ���� �����Ǿ� �ִٸ�
    else
    {
        m_accArrowTime += DT;
        if (G_ARROW_REMAIN_TIME <= m_accArrowTime)
        {
            GetOwner()->SetInvalid();
        }
    }
}
