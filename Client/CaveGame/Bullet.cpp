#include "pch.h"
#include "Bullet.h"
#include "MeshRenderer.h"
#include "AssimpMgr.h"
#include "Transform.h"
#include "Collider.h"

Bullet::Bullet(const glm::vec3 initPos,const glm::vec3 initDir)
{
	auto m = make_shared<MeshRenderer>();
	m->SetModelData(Mgr(AssimpMgr)->LoadModel("MySphereRed.fbx"));
	m->SetShader("SimpleShaderHasColorLight.glsl");
	m_arrComp[etoi(COMPONENT_TYPE::MESH_RENDERER)] = std::move(m);
	const auto pTrans = GetTransform();
	pTrans->SetLocalScale(0.001f);
	pTrans->SetLocalPosition(initPos);
	m_bulletDir = initDir;
	auto pCol = AddComponent<Collider>();
	pCol->SetColBoxScale({ 10,10,10 });
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
	GetTransform()->AddPosition(10.f * DT * m_bulletDir);
	GameObj::Update();
}
