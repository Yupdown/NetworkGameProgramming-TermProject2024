#include "pch.h"
#include "ProjectileFireball.h"
#include "MCTilemap.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Model.h"
#include "Mesh.h"
#include "SoundMgr.h"

ProjectileFireball::ProjectileFireball(shared_ptr<MCTilemap> tilemap) : m_refTilemap(tilemap), ServerObject()
{
}

void ProjectileFireball::Start()
{
	ServerObject::Start();

	m_renderer = AddComponent<MeshRenderer>();

	vector<Vertex> vertices;
	vector<GLuint> indices;
	for (int i = 0; i < 4; ++i)
	{
		Vertex v;
		v.position = glm::vec3(0.5f - i % 2, i / 2 - 0.5f, 0.0f);
		v.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		v.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
		v.uv = glm::vec2(i % 2, i / 2);
		vertices.emplace_back(v);
	}
	indices = { 0, 1, 2, 1, 3, 2 };
	auto mesh = make_shared<Mesh>();
	mesh->Init(std::move(vertices), std::move(indices));

	shared_ptr<Model> model = make_shared<Model>();
	auto material = make_shared<Material>();
	material->AddTexture2D("fireball.png");
	model->AddMesh(mesh);
	model->AddMaterial(material);

	m_renderer->SetModelData(model);
	m_renderer->SetShader("BillboardShader.glsl");

	Mgr(SoundMgr)->PlayEffect("Ghast_fireball.ogg", 1.0f);
}

void ProjectileFireball::Update()
{
	// move forward with no gravity
	glm::vec3 pos_pre = GetPosition();
	glm::vec3 v = GetVelocity();
	glm::vec3 pos_post = pos_pre + v * DT;

	// check collision with tilemap
	glm::ivec3 tilePos = pos_post;
	if (m_refTilemap->GetTile(tilePos) != 0 && !m_isPinned)
	{
		// hit the wall
		v = glm::zero<glm::vec3>();
		Mgr(SoundMgr)->PlayEffect("Explosion.ogg", 1.0f);

		m_isPinned = true;
	}
	if (m_isPinned)return;

	SetPosition(pos_post);
	SetVelocity(v);

	ServerObject::Update();
}
