#include "pch.h"
#include "Mesh.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "Player.h"
#include "PlayerCam.h"
#include "AssimpMgr.h"
#include "Transform.h"
#include "Camera.h"
#include "AssimpMgr.h"
#include "Bullet.h"
#include "EventMgr.h"
#include "RayCaster.h"
#include "MeshRenderer.h"

#include "ParticleMgr.h"
#include "Material.h"
#include "PlayerShadow.h"
#include "MCTilemap.h"
#include "SoundMgr.h"
#include <ResMgr.h>
#include <MyOpenGL.hpp>

extern std::atomic_bool g_bTileFinish;
static std::uniform_int_distribution tile_num{ 1,12 };

shared_ptr<GameObj> Player::CreateCursorBlockObj() const
{
	shared_ptr<GameObj> obj = make_obj<GameObj>();
	auto meshRenderer = obj->AddComponent<MeshRenderer>();

	vector<Vertex> vertices;
	for (int i = 0; i < 8; ++i)
	{
		Vertex v;
		v.position = glm::vec3(i % 2, i / 2 % 2, i / 4 % 2) - glm::one<glm::vec3>() * 0.5f;
		v.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		vertices.push_back(v);
	}
	vector<GLuint> indices{
		0, 1, 0, 2, 3, 1, 3, 2,
		0, 4, 1, 5, 2, 6, 3, 7,
		4, 5, 4, 6, 7, 5, 7, 6
	};

	shared_ptr<Mesh> mesh = make_shared<Mesh>(vertices, indices);
	mesh->SetPolygonMode(GL_LINES);
	mesh->SetBuffers();
	meshRenderer->AddMesh(mesh);
	meshRenderer->SetShader("CursorBlockShader.glsl");

	return obj;
}

void Player::ChangeCamType() noexcept
{

}

void Player::UpdatePlayerCamFpsMode() noexcept
{
	const auto camTrans = m_cameraAnchor->GetTransform();
	glm::vec2 offset = Mgr(KeyMgr)->GetMouseDelta() * m_fCamSensivity;

	m_cameraAngleAxis += glm::vec3(offset.y, offset.x, 0.0f);
	m_cameraAngleAxis.x = glm::clamp(m_cameraAngleAxis.x, -89.0f, 89.0f);

	m_cameraAngleAxisSmooth = m_cameraAngleAxisSmooth + (m_cameraAngleAxis - m_cameraAngleAxisSmooth) * Mgr(TimeMgr)->GetDT() * 16.0f;
	camTrans->SetLocalRotation(glm::quat(glm::radians(m_cameraAngleAxisSmooth)));
}

void Player::InitCamDirection() noexcept
{
	m_cameraAngleAxis = glm::zero<glm::vec3>();
	m_cameraAngleAxisSmooth = glm::zero<glm::vec3>();
	m_cameraAnchor->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
	m_rendererObj->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
	m_pCamera->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
	GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
}

void Player::UpdateTileManipulation()noexcept
{
	if (!g_bTileFinish)
	{
		return;
	}
	const glm::vec3 wv = m_cameraAnchor->GetTransform()->GetWorldPosition() * 10.0f;
	const RaycastResult result = m_refTilemap->RaycastTile(wv, this->GetPlayerLook(), 10.0f);
	m_cursorBlockObj->GetTransform()->SetLocalPosition((glm::vec3(result.hitTilePosition) + glm::one<glm::vec3>() * 0.5f) / 10.0f - GetTransform()->GetLocalPosition());

	if (!result.hit)
		return;

	if (KEY_TAP(GLFW_MOUSE_BUTTON_LEFT))
	{
		// 쓰고있는 텍스쳐의 이름 또는 인덱스, 아니면 텍스쳐포인터를 알면됌
		const string tex_name = std::format("tile_{}.png", tile_num(g_rng));
		auto iter = g_mapParticleUniqueObject.find(tex_name);
		shared_ptr<Material>mate;
		if (g_mapParticleUniqueObject.end() == iter)
		{
			mate = make_shared<Material>();
			mate->AddTexture2D(tex_name);
			auto particle = make_shared<GameObj>(*m_particlePrefab);
			particle->GetComp<MeshRenderer>()->AddMaterial(mate);
			particle->SetResName(tex_name);
			iter = g_mapParticleUniqueObject.emplace(tex_name, particle).first;
		}
		
		Mgr(InstancingMgr)->AddInstancingList(iter->second);
		if (mate)
		{
			Mgr(InstancingMgr)->SetAllObjMaterials(tex_name, mate);
		}
		m_refTilemap->SetTile(result.hitTilePosition, 0, true);
		Mgr(ParticleMgr)->SetParticles(iter->second, 0.01f, result.hitPosition * 0.1f);
		DestroyObj(iter->second);
		Mgr(SoundMgr)->PlayEffect("stone4.ogg", 0.5f);
	}
	if (KEY_TAP(GLFW_MOUSE_BUTTON_RIGHT))
	{
		const int tileID = Mgr(UIMgr)->GetSelectIndex() + 1;
		m_refTilemap->SetTile(result.hitTilePosition + glm::ivec3(result.hitNormal), tileID, true);
		Mgr(SoundMgr)->PlayEffect("stone4.ogg", 0.5f);
	}
}

void Player::MoveByView(const glm::vec3& vDelta)
{
	glm::vec3 vWorldDelta = glm::quat(glm::vec3(0.0f, glm::radians(m_cameraAngleAxis.y), 0.0f)) * vDelta;
	m_vAccelation += vWorldDelta;
	m_rendererBodyAngleY = Lerp(m_rendererBodyAngleY, m_cameraAngleAxis.y, DT * 8.0f);
}

void Player::UpdateRenderer()
{
	float rotationFactor = glm::length(m_vVelocity) * sin(m_fMoveTime * 10.0f) * glm::pi<float>() * 0.25f;

	m_rendererBodyAngleY = glm::clamp(m_rendererBodyAngleY, m_cameraAngleAxisSmooth.y - 30.0f, m_cameraAngleAxisSmooth.y + 30.0f);
	m_rendererObj->GetTransform()->SetLocalRotation(glm::quat(glm::vec3(0.0f, glm::radians(m_rendererBodyAngleY - 90.0f), 0.0f)));

	glm::quat headRotation = glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(m_cameraAngleAxisSmooth.y - m_rendererBodyAngleY), 0.0f));
	headRotation = glm::rotate(headRotation, glm::radians(glm::clamp(-m_cameraAngleAxisSmooth.x, -60.0f, 60.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	m_transformHead->SetLocalRotation(headRotation);
	m_transformHeadOut->SetLocalRotation(headRotation);

	m_transformLArm->SetLocalPosition(glm::vec3(rotationFactor * 120.0f, 0.0f, m_transformLArm->GetLocalPosition().z));
	m_transformLArmOut->SetLocalPosition(glm::vec3(rotationFactor * 120.0f, 0.0f, m_transformLArmOut->GetLocalPosition().z));
	m_transformRArm->SetLocalPosition(glm::vec3(-rotationFactor * 120.0f, 0.0f, m_transformRArm->GetLocalPosition().z));
	m_transformRArmOut->SetLocalPosition(glm::vec3(-rotationFactor * 120.0f, 0.0f, m_transformRArmOut->GetLocalPosition().z));
	m_transformLLeg->SetLocalPosition(glm::vec3(-rotationFactor * 120.0f, m_transformLLeg->GetLocalPosition().y, m_transformLLeg->GetLocalPosition().z));
	m_transformLLegOut->SetLocalPosition(glm::vec3(-rotationFactor * 120.0f, m_transformLLegOut->GetLocalPosition().y, m_transformLLegOut->GetLocalPosition().z));
	m_transformRLeg->SetLocalPosition(glm::vec3(rotationFactor * 120.0f, m_transformRLeg->GetLocalPosition().y, m_transformRLeg->GetLocalPosition().z));
	m_transformRLegOut->SetLocalPosition(glm::vec3(rotationFactor * 120.0f, m_transformRLegOut->GetLocalPosition().y, m_transformRLegOut->GetLocalPosition().z));

	m_transformLArm->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, rotationFactor)));
	m_transformLArmOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, rotationFactor)));
	m_transformRArm->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, -rotationFactor)));
	m_transformRArmOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, -rotationFactor)));
	m_transformLLeg->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, -rotationFactor)));
	m_transformLLegOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, -rotationFactor)));
	m_transformRLeg->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, rotationFactor)));
	m_transformRLegOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, rotationFactor)));
}

void Player::UpdateCameraTransform(shared_ptr<Transform> pCameraTransfrom)
{
	glm::vec3 wv = m_cameraAnchor->GetTransform()->GetWorldPosition() * 10.0f;
	const float fMaxDist = 5.0f;
	switch (m_curCamMode)
	{
	case 0:
		pCameraTransfrom->SetLocalPosition(glm::zero<glm::vec3>());
		pCameraTransfrom->SetLocalRotation(glm::identity<glm::quat>());
		break;
	case 1:
	{
		const RaycastResult result = m_refTilemap->RaycastTile(wv, -this->GetPlayerLook(), fMaxDist);
		float target = -(result.hit ? glm::distance(wv, result.hitPosition) : fMaxDist);
		pCameraTransfrom->SetLocalPosition(glm::vec3(0.0f, 0.0f, glm::max(target, glm::mix(pCameraTransfrom->GetLocalPosition().z / 0.1f, target, DT * 8.0f))) * 0.1f);
		pCameraTransfrom->SetLocalRotation(glm::identity<glm::quat>());
	}
		break;
	case 2:
	{
		const RaycastResult result = m_refTilemap->RaycastTile(wv, this->GetPlayerLook(), fMaxDist);
		float target = result.hit ? glm::distance(wv, result.hitPosition) : fMaxDist;
		pCameraTransfrom->SetLocalPosition(glm::vec3(0.0f, 0.0f, glm::min(target, glm::mix(pCameraTransfrom->GetLocalPosition().z / 0.1f, target, DT * 8.0f))) * 0.1f);
		pCameraTransfrom->SetLocalRotation(glm::quat(glm::vec3(0.0f, glm::pi<float>(), 0.0f)));
		break;
	}
	}
	float tParam = m_fMoveTime * 10.0f;
	float mParam = 0.004f;
	pCameraTransfrom->SetLocalPosition(glm::vec3(glm::sin(tParam) * mParam, glm::sin(tParam * 2.0f) * mParam, pCameraTransfrom->GetLocalPosition().z));
}

Player::Player(shared_ptr<MCTilemap> tilemap) : m_refTilemap(tilemap)
{
	m_rendererObj = Mgr(AssimpMgr)->LoadAllPartsAsGameObj("DefaultWarpShader.glsl", "Player.fbx");
	m_rendererObj->GetTransform()->SetLocalPosition(glm::vec3(0.0f, 0.11f, 0.0f));
	m_rendererObj->GetTransform()->SetLocalScale(0.0003f);

	m_transformHead = m_rendererObj->FindChildObj("Head")->GetTransform();
	m_transformHeadOut = m_rendererObj->FindChildObj("HeadOut")->GetTransform();
	m_transformLArm = m_rendererObj->FindChildObj("LArm")->GetTransform();
	m_transformLArmOut = m_rendererObj->FindChildObj("LArmOut")->GetTransform();
	m_transformRArm = m_rendererObj->FindChildObj("RArm")->GetTransform();
	m_transformRArmOut = m_rendererObj->FindChildObj("RArmOut")->GetTransform();
	m_transformLLeg = m_rendererObj->FindChildObj("LLeg")->GetTransform();
	m_transformLLegOut = m_rendererObj->FindChildObj("LLegOut")->GetTransform();
	m_transformRLeg = m_rendererObj->FindChildObj("RLeg")->GetTransform();
	m_transformRLegOut = m_rendererObj->FindChildObj("RLegOut")->GetTransform();

	m_fpChangeCamMode[0] = [this]() noexcept {
		m_cameraObj->GetTransform()->SetLocalPosition(glm::zero<glm::vec3>());
		m_cameraObj->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
		};
	m_fpChangeCamMode[1] = [this]() noexcept {
		m_cameraObj->GetTransform()->SetLocalPosition(glm::zero<glm::vec3>());
		m_cameraObj->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
		};
	m_fpChangeCamMode[2] = [this]() noexcept {
		m_cameraObj->GetTransform()->SetLocalPosition(glm::zero<glm::vec3>());
		m_cameraObj->GetTransform()->SetLocalRotation(glm::quat(glm::vec3(0.0f, glm::pi<float>(), 0.0f)));
		};

	m_cameraAnchor = make_obj<GameObj>();
	m_cameraAnchor->GetTransform()->SetLocalPosition(glm::vec3(0.0f, 0.18f, 0.0f));

	m_cameraObj = make_obj<GameObj>();
	m_fpChangeCamMode[m_curCamMode]();

	m_pCamera = m_cameraObj->AddComponent<Camera>();
	m_pCamera->SetNear(1e-3f);
	m_pCamera->SetMainCam();

	m_cursorBlockObj = CreateCursorBlockObj();
	m_cursorBlockObj->GetTransform()->SetLocalScale(glm::one<glm::vec3>() * 0.105f);

	m_particlePrefab = Mgr(AssimpMgr)->Load("DefaultShader.glsl", "MyCube.fbx");
	m_particlePrefab->GetTransform()->SetLocalScale(0.1f);
}

Player::~Player()
{

}

void Player::Start()
{
	InitCamDirection();

	AddChild(m_rendererObj);
	AddChild(m_cameraAnchor);
	AddChild(m_cursorBlockObj);
	AddChild(make_shared<PlayerShadow>(m_refTilemap));
	m_cameraAnchor->AddChild(m_cameraObj);

	for (auto& child : *m_rendererObj)
	{
		for (auto& mate : child->GetComp<MeshRenderer>()->GetMaterial())
		{
			mate->SetMaterialAmbient({ 10,10,10 });
			mate->SetMaterialDiffuse({ 10,10,10 });
			mate->SetMaterialSpecular({ 10,10,10 });
		}
	}
	GameObj::Start();
}

void Player::Update()
{
	const auto pPlayerTrans = GetTransform();

	m_vAccelation = glm::vec3(0.0f, -4.0f, 0.0f);

	if (KEY_HOLD(GLFW_KEY_A))
	{
		MoveByView(glm::vec3(-1.0f, 0.0f, 0.0f) * 10.0f);
	}
	if (KEY_HOLD(GLFW_KEY_W))
	{
		MoveByView(glm::vec3(0.0f, 0.0f, 1.0f) * 10.0f);
	}
	if (KEY_HOLD(GLFW_KEY_S))
	{
		MoveByView(glm::vec3(0.0f, 0.0f, -1.0f) * 10.0f);
	}
	if (KEY_HOLD(GLFW_KEY_D))
	{
		MoveByView(glm::vec3(1.0f, 0.0f, 0.0f) * 10.0f);
	}
	if (KEY_HOLD(GLFW_KEY_Q))
	{
		m_cameraAngleAxis.y -= 50.0f * DT;
	}
	if (KEY_HOLD(GLFW_KEY_E))
	{
		m_cameraAngleAxis.y += 50.0f * DT;
	}
	if (KEY_HOLD(GLFW_KEY_SPACE))
	{
		if (m_bGround)
			m_vVelocity += glm::vec3(0.0f, 1.0f, 0.0f);
		// MoveByView(glm::vec3(0.0f, 1.0f, 0.0f) * 10.0f);
	}
	if (KEY_HOLD(GLFW_KEY_LEFT_SHIFT))
	{
		MoveByView(glm::vec3(0.0f, -1.0f, 0.0f) * 10.0f);
	}
	if (KEY_TAP(GLFW_KEY_F5))
	{
		m_curCamMode = wrapAround(m_curCamMode + 1, 0, 3);
		m_fpChangeCamMode[m_curCamMode]();
	}

	m_vVelocity = m_vVelocity + m_vAccelation * DT;

	glm::vec2 vVelocityXZ = glm::vec2(m_vVelocity.x, m_vVelocity.z);
	float l = glm::length(vVelocityXZ);
	if (l > m_vVelocityMax)
		vVelocityXZ *= m_vVelocityMax / l;
	if (l > 0.0f)
		vVelocityXZ = vVelocityXZ - glm::normalize(vVelocityXZ) * glm::min(l, 4.0f * DT);
	m_vVelocity.x = vVelocityXZ.x;
	m_vVelocity.z = vVelocityXZ.y;

	glm::vec3 position = GetTransform()->GetLocalPosition() * 10.0f;
	glm::vec3 positionPost = position + m_vVelocity * DT * 10.0f;

	bool old_bGround = m_bGround;
	m_bGround = m_refTilemap->HandleCollision(position, positionPost, m_vVelocity);
	GetTransform()->SetLocalPosition(positionPost * 0.1f);

	float old_fMoveTime = m_fMoveTime;
	m_fMoveTime += glm::length(m_vVelocity) * DT;

	if (m_bGround && glm::mod(old_fMoveTime, 0.25f) > glm::mod(m_fMoveTime, 0.25f))
		Mgr(SoundMgr)->PlayEffect(std::format("grass{}.ogg", (int)(m_fMoveTime * 4) % 4 + 1), 0.25f);

	if (!old_bGround && m_bGround)
		Mgr(SoundMgr)->PlayEffect("grass2.ogg", 0.25f);

	UpdatePlayerCamFpsMode();
	UpdateTileManipulation();
	UpdateCameraTransform(m_cameraObj->GetTransform());
	GameObj::Update();

	//TODO 임시
	const auto [target, point] = Mgr(RayCaster)->GetPickedObjAndPoint();
	if (target)
	{
		Mgr(ParticleMgr)->SetParticles(target, 0.1f, point);
	}

	m_pCamera->SetCamFov(Lerp(m_pCamera->GetCamFov(), glm::radians(KEY_HOLD(GLFW_KEY_LEFT_CONTROL) ? 60.0f : 45.0f), DT * 8.0f));
	UpdateRenderer();
}

glm::vec3 Player::GetPlayerLook() const noexcept
{
	return glm::rotate(glm::quat(glm::vec3(glm::radians(m_cameraAngleAxisSmooth.x), glm::radians(m_cameraAngleAxisSmooth.y), 0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Player::Fire()  noexcept
{
	auto bullet = make_shared<Bullet>(GetTransform()->GetWorldPosition(), GetPlayerLook());
	CreateObj(std::move(bullet), GROUP_TYPE::PROJ_PLAYER);
}
