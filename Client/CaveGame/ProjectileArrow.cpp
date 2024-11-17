#include "pch.h"
#include "ProjectileArrow.h"
#include "MCTilemap.h"

#include <AssimpMgr.h>
#include <Material.h>
#include <MeshRenderer.h>

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
