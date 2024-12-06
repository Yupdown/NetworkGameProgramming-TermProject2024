#include "pch.h"
#include "EnderEye.h"
#include "GameObj.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Model.h"
#include "Mesh.h"
#include "Transform.h"
#include "SoundMgr.h"

EnderEye::EnderEye()
{
}

EnderEye::~EnderEye()
{
}

void EnderEye::Start()
{
	GameObj::Start();

	renderer = AddComponent<MeshRenderer>();

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
	material = make_shared<Material>();
	material->AddTexture2D("item_eye.png");
	model->AddMesh(mesh);
	model->AddMaterial(material);

	renderer->SetModelData(model);
	renderer->SetShader("BillboardShader.glsl");
}

void EnderEye::Update()
{
	m_fTimer -= DT;
	if (m_fTimer < 0.0f)
	{
		GetTransform()->SetLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));
	}
	else
	{
		float t = 1.0f - m_fTimer / 10.0f;
		t = cos(t * 3.141592f) * -0.5f + 0.5f;
		GetTransform()->SetLocalPosition(glm::mix(m_beginPos, m_endPos, t));
	}
}

void EnderEye::SetMoveSequence(const glm::vec3& beginPos, const glm::vec3& endPos)
{
	m_beginPos = beginPos;
	m_fTimer = 10.0f;

	constexpr float MAX_DIST = 10.0f;
	glm::vec3 delta = endPos - beginPos;
	delta.y = 0.0f;
	if (delta.length() > MAX_DIST)
	{
		m_endPos = beginPos + glm::normalize(delta) * MAX_DIST + glm::vec3(0.0f, 10.0f, 0.0f);
	}
	else
	{
		m_endPos = endPos;
	}

	Mgr(SoundMgr)->PlayEffect("Ender_Eye_launch1.ogg", 1.0f);
}
