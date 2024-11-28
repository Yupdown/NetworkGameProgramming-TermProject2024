#include "pch.h"
#include "ProjectileArrow.h"
#include "MCTilemap.h"

#include <AssimpMgr.h>
#include <Material.h>
#include <MeshRenderer.h>
#include <ServerObjectManager.h>
#include <EventHandler.h>

ProjectileArrow::ProjectileArrow(shared_ptr<MCTilemap> tilemap) : ServerObject()
{
    m_refTilemap = tilemap;
}

void ProjectileArrow::Awake()
{
    ServerObject::Awake();

    auto pArrow = Mgr(AssimpMgr)->Load("DefaultWarpShader.glsl", "projectile_arrow.obj");
    pArrow->GetTransform()->SetLocalPosition(glm::vec3(0.0f, 0.0f, 0.25f));
    pArrow->GetTransform()->SetLocalRotation(90.0f, Y_AXIS);
    pArrow->GetTransform()->SetLocalScale(2.0f);
    AddChild(pArrow);

    shared_ptr<Material> arrowMaterial = make_shared<Material>();
    arrowMaterial->AddTexture2D("arrow.png");

    auto arrowRenderer = pArrow->GetComp<MeshRenderer>();
    arrowRenderer->AddMaterial(arrowMaterial);
}

void ProjectileArrow::Update()
{
    // 화살이 벽에 고정되어 있지 않다면
    if (!m_isPinned)
    {
        // 물리 연산 수행 및 적용
        float gravity = 9.8f * 2.0f;
        glm::vec3 pos_pre = GetPosition();
        glm::vec3 v = GetVelocity();
        v.y -= gravity * DT;
        glm::vec3 pos_post = pos_pre + v * DT;

        glm::ivec3 tilePos = pos_post;
        if (m_refTilemap->GetTile(tilePos) != 0)
        {
            m_isPinned = true;
            v = glm::zero<glm::vec3>();
        }

        SetPosition(pos_post);
        SetVelocity(v);

        // 화살이 나아가는 방향에 따라 회전
        if (v.x != 0.0f || v.y != 0.0f || v.z != 0.0f)
        {
            glm::vec3 n = glm::normalize(v);
            glm::quat lookRotation = glm::quatLookAt(n, glm::vec3(0.0f, 1.0f, 0.0f));
            SetRotation(lookRotation);
        }

        // 각 그룹에 대해 충돌 검사를 수행할 람다 함수
        auto group_collision_func = [this](GROUP_TYPE type, float radius, float height) {
                auto& monster_group = Mgr(ServerObjectManager)->GetTargetScene()->GetGroupObj(type);
                for (auto& monster : monster_group)
                {
                    const auto& monster_pos = monster->GetTransform()->GetLocalPosition();
                    const auto& project_pos = GetTransform()->GetLocalPosition();
                    const float collision_radius = radius;
                    const float collision_height = height;

                    // 화살 충돌 여부 계산 (점 대 원기둥 충돌)
                    bool is_hit = true;
                    is_hit &= glm::distance(glm::vec2(monster_pos.x, monster_pos.z), glm::vec2(project_pos.x, project_pos.z)) <= collision_radius;
                    is_hit &= project_pos.y >= monster_pos.y && project_pos.y <= monster_pos.y + collision_height;
                    if (is_hit)
                    {
                        // 오브젝트에게 대미지를 입힘
                        shared_ptr<ServerObject> serverObj = dynamic_pointer_cast<ServerObject>(monster);
                        serverObj->OnObjectDamaged(100);

                        // 화살 삭제; 서버오브젝트로 전환시 ServerObjectManager::RemoveObject() 호출
                        DestroyObj(shared_from_this());
                        return;
                    }
                }
            };
        group_collision_func(GROUP_TYPE::MONSTER, 0.5f, 3.0f);
        group_collision_func(GROUP_TYPE::PLAYER, 0.5f, 2.0f);
    }
    // 화살이 벽에 고정되어 있다면
    else
    {
        // 타일 정보를 가져와서 화살이 고정되어 있는지 확인
        glm::ivec3 tilePos = GetPosition();
        // 타일이 비어있다면 고정을 해제
        if (m_refTilemap->GetTile(tilePos) == 0)
			m_isPinned = false;
    }

	ServerObject::Update();
}
