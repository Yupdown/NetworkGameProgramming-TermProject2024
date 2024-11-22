#include "pch.h"
#include "DropItem.h"
#include "GameObj.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Model.h"
#include "Mesh.h"
#include "Transform.h"
#include "MCTilemap.h"
#include "MCItem.h"

DropItem::DropItem(shared_ptr<MCTilemap> tilemap, MCItem* item, int stack_size)
{
	m_refTilemap = tilemap;
	m_item = item;
	m_stackSize = stack_size;
}

MCItem* DropItem::GetItem() const
{
	return m_item;
}

int DropItem::GetStackSize() const
{
	return m_stackSize;
}

void DropItem::Start()
{
	ServerObject::Start();

	vector<Vertex> vertices;
	vector<GLuint> indices;
	for (int i = 0; i < 4; ++i)
	{
		Vertex v;
		v.position = glm::vec3(0.5f - i % 2, i / 2 - 0.5f, 0.0f) * 0.5f;
		v.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		v.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
		v.uv = glm::vec2(i % 2, i / 2);
		vertices.emplace_back(v);
	}
	indices = { 0, 1, 2, 1, 3, 2 };

	m_rendererObject = make_obj<GameObj>();
	m_renderer = m_rendererObject->AddComponent<MeshRenderer>();

	auto mesh = make_shared<Mesh>();
	mesh->Init(std::move(vertices), std::move(indices));
	shared_ptr<Model> model = make_shared<Model>();
	m_material = make_shared<Material>();
	m_material->AddTexture2D(m_item->GetIconTexture());
	model->AddMesh(mesh);
	model->AddMaterial(m_material);
	m_renderer->SetModelData(model);
	m_renderer->SetShader("BillboardShader.glsl");

	AddChild(m_rendererObject);
}

void DropItem::Update()
{
	// 드랍 아이템 애니메이션; sin 함수를 이용해 위아래로 움직이도록 함
	m_timer += DT;
	m_rendererObject->GetTransform()->SetLocalPosition(glm::vec3(0.0f, 0.25f + sin(m_timer * 8.0f) / 16.0f, 0.0f));

	// 물리 연산 수행 및 적용
	float gravity = 9.8f * 2.0f;
	glm::vec3 pos_pre = GetPosition();
	glm::vec3 v = GetVelocity();
	v.y -= gravity * DT;
	glm::vec3 pos_post = pos_pre + v * DT;

	bool ground = m_refTilemap->HandleCollision(pos_pre, pos_post, v, 0.125f, 0.25f);
	SetPosition(pos_post);
	SetVelocity(v);
	
	ServerObject::Update();
}