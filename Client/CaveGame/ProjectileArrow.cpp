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
    // ȭ���� ���� �����Ǿ� ���� �ʴٸ�
    if (!m_isPinned)
    {
        // ���� ���� ���� �� ����
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

        // ȭ���� ���ư��� ���⿡ ���� ȸ��
        if (v.x != 0.0f || v.y != 0.0f || v.z != 0.0f)
        {
            glm::vec3 n = glm::normalize(v);
            glm::quat lookRotation = glm::quatLookAt(n, glm::vec3(0.0f, 1.0f, 0.0f));
            SetRotation(lookRotation);
        }

        // �� �׷쿡 ���� �浹 �˻縦 ������ ���� �Լ�
        auto group_collision_func = [this](GROUP_TYPE type, float radius, float height) {
                auto& monster_group = Mgr(ServerObjectManager)->GetTargetScene()->GetGroupObj(type);
                for (auto& monster : monster_group)
                {
                    const auto& monster_pos = monster->GetTransform()->GetLocalPosition();
                    const auto& project_pos = GetTransform()->GetLocalPosition();
                    const float collision_radius = radius;
                    const float collision_height = height;

                    // ȭ�� �浹 ���� ��� (�� �� ����� �浹)
                    bool is_hit = true;
                    is_hit &= glm::distance(glm::vec2(monster_pos.x, monster_pos.z), glm::vec2(project_pos.x, project_pos.z)) <= collision_radius;
                    is_hit &= project_pos.y >= monster_pos.y && project_pos.y <= monster_pos.y + collision_height;
                    if (is_hit)
                    {
                        // ������Ʈ���� ������� ����
                        shared_ptr<ServerObject> serverObj = dynamic_pointer_cast<ServerObject>(monster);
                        serverObj->OnObjectDamaged(100);

                        // ȭ�� ����; ����������Ʈ�� ��ȯ�� ServerObjectManager::RemoveObject() ȣ��
                        DestroyObj(shared_from_this());
                        return;
                    }
                }
            };
        group_collision_func(GROUP_TYPE::MONSTER, 0.5f, 3.0f);
        group_collision_func(GROUP_TYPE::PLAYER, 0.5f, 2.0f);
    }
    // ȭ���� ���� �����Ǿ� �ִٸ�
    else
    {
        // Ÿ�� ������ �����ͼ� ȭ���� �����Ǿ� �ִ��� Ȯ��
        glm::ivec3 tilePos = GetPosition();
        // Ÿ���� ����ִٸ� ������ ����
        if (m_refTilemap->GetTile(tilePos) == 0)
			m_isPinned = false;
    }

	ServerObject::Update();
}
