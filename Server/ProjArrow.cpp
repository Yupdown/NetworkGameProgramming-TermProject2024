#include "pch.h"
#include "ProjArrow.h"
#include "Object.h"
#include "MCTilemap.h"
#include "MCWorld.h"

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
            const auto project_pos = pArrow->GetOwner()->GetPos();
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
                    pArrow->GetOwner()->SetInvalid();
                    return;
                }
            }
            };
        if (m_isPinned)return;
        group_collision_func(this, MC_OBJECT_TYPE::MONSTER, 0.5f, 3.0f);
        group_collision_func(this, MC_OBJECT_TYPE::PLAYER, 0.5f, 2.0f);
    }
    // 화살이 벽에 고정되어 있다면
    else
    {
       //// 타일 정보를 가져와서 화살이 고정되어 있는지 확인
       //glm::ivec3 tilePos = GetPosition();
       //// 타일이 비어있다면 고정을 해제
       //if (m_refTilemap->GetTile(tilePos) == 0)
       //    m_isPinned = false;
    }
}
