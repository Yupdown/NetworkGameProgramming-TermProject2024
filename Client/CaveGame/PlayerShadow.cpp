#include "pch.h"
#include "PlayerShadow.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Core.h"
#include "Shader.h"
#include "MCTilemap.h"

PlayerShadow::PlayerShadow(const shared_ptr<const MCTilemap>& tilemap)
    :m_pTilemap{ tilemap }
{
}

PlayerShadow::~PlayerShadow()
{
}

void PlayerShadow::Start()
{
    const int numSegments = 40; 
    const float radius = 1.0f; 
    
    vector<Vertex> vertices;
    vector<GLuint> indices;

    for (int i = 0; i <= numSegments; ++i)
    {
        Vertex v;
        const float angle = 2.0f * glm::pi<float>() * float(i) / float(numSegments);
        const float x = radius * cos(angle);
        const float z = radius * sin(angle);

        v.position = glm::vec3(x, 0.01f, z);
        v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        vertices.emplace_back(v);
    }

    for (int i = 1; i <= numSegments; ++i)
    {
        indices.emplace_back(0);
        indices.emplace_back(i % numSegments + 1);
        indices.emplace_back(i);
    }
    auto mesh = make_shared<Mesh>(vertices, indices);
    mesh->SetBuffers();
    auto mr = AddComponent<MeshRenderer>();
    mr->AddMesh(mesh);
    mr->SetShader("ShadowShader.glsl");
    GetTransform()->SetLocalScale(0.05f);

    GetParentGameObj().lock()->GetTransform()->MakeFinalMat();

    m_fPlayerOriginY = GetParentGameObj().lock()->GetTransform()->GetWorldPosition().y;
    m_fShadowAlpha = 0.5f;
}

void PlayerShadow::Update()
{
    const auto pParentTrans = GetParentGameObj().lock()->GetTransform();
    const auto pTrans = GetTransform();

    const glm::vec3 worldPosition = pParentTrans->GetLocalPosition() * 10.0f;
    glm::ivec3 tilePosition = glm::floor(worldPosition);

    tilePosition.x = glm::clamp(tilePosition.x, 0, MCTilemap::MAP_WIDTH - 1);
    tilePosition.y = glm::clamp(tilePosition.y, 0, MCTilemap::MAP_HEIGHT - 1);
    tilePosition.z = glm::clamp(tilePosition.z, 0, MCTilemap::MAP_WIDTH - 1);

    int y = tilePosition.y;
    while (y >= 0)
    {
        if (m_pTilemap->GetTile(tilePosition.x, y, tilePosition.z) > 0)
            break;
        --y;
    }

    const float distance = worldPosition.y - y - 1;
    pTrans->SetLocalPosition(glm::vec3(0.0f, -distance / 10.0f + 0.005f, 0.0f));

    const float fDelta = 0.1f * (pParentTrans->GetWorldPosition().y - m_fPlayerOriginY);
    pTrans->SetLocalScale(0.03f / (1.0f + glm::abs(distance)));
    GameObj::Update();
}

void PlayerShadow::Render()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const auto& shader = GetComp<MeshRenderer>()->GetShader();
    shader->Use();
    GLint progID;
    glGetIntegerv(GL_CURRENT_PROGRAM, &progID);
    glUniform1f(glGetUniformLocation(progID, "uShadowAlpha"),m_fShadowAlpha);
    GameObj::Render();

    glDisable(GL_BLEND);
}