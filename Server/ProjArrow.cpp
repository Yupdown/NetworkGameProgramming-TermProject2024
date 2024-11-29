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
       
        // 각 그룹에 대해 충돌 검사를 수행할 람다 함수
        constexpr const auto group_collision_func = [](ProjArrow* const pArrow, MC_OBJECT_TYPE type, float radius, float height)noexcept {
            const auto& monster_group = Mgr(MCWorld)->GetWorldObjects(type);
            const auto owner = pArrow->GetOwner();
            const auto project_pos = owner->GetPos();
            for (const auto& monster : monster_group)
            {
                const auto& monster_pos = monster->GetPos();
                const float collision_radius = radius;
                const float collision_height = height;

                // 화살 충돌 여부 계산 (점 대 원기둥 충돌)
                bool is_hit = true;
                is_hit &= glm::distance(glm::vec2(monster_pos.x, monster_pos.z), glm::vec2(project_pos.x, project_pos.z)) <= collision_radius;
                is_hit &= project_pos.y >= monster_pos.y && project_pos.y <= monster_pos.y + collision_height;
                if (is_hit)
                {
                    // 오브젝트에게 대미지를 입힘
                    // 몬스터가 확실하니 믿고 static
                    //const auto serverObj = static_cast<ServerObject*>(monster.get());
                    //serverObj->OnObjectDamaged(ARROW_DMG);

                    // 화살 삭제; 서버오브젝트로 전환시 ServerObjectManager::RemoveObject() 호출
                    //DestroyObj(std::move(pArrow));
                    std::cout << "충돌!\n";
                    monster->DecHP(G_ARROW_DMG);
                    owner->SetInvalid();
                    return;
                }
            }
            };
        if (m_isPinned)return;
        // TODO: 분리하고 객체화
        group_collision_func(this, MC_OBJECT_TYPE::MONSTER, 0.5f, 3.0f);
        group_collision_func(this, MC_OBJECT_TYPE::PLAYER, 0.5f, 2.0f); 
    }
    // 화살이 벽에 고정되어 있다면
    else
    {
        m_accArrowTime += DT;
        if (G_ARROW_REMAIN_TIME <= m_accArrowTime)
        {
            GetOwner()->SetInvalid();
        }
    }
}
