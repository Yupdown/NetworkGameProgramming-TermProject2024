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
#include "Monster.h"

#include "ParticleMgr.h"
#include "Material.h"
#include "PlayerShadow.h"
#include "MCTilemap.h"
#include "SoundMgr.h"
#include <ResMgr.h>
#include <MyOpenGL.hpp>
#include "Model.h"
#include "Hero.h"

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

void Player::InitCamDirection() noexcept
{
	m_cameraAngleAxis = glm::zero<glm::vec3>();
	m_cameraAngleAxisSmooth = glm::zero<glm::vec3>();
	m_cameraAnchor->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
	m_rendererObj->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
	m_pCamera->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
	GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
}

void Player::InitializeRenderer()
{
	m_rendererObj = Mgr(AssimpMgr)->LoadAllPartsAsGameObj("DefaultWarpShader.glsl", "Player.fbx");
	m_rendererObj->GetTransform()->SetLocalPosition(glm::vec3(0.0f, 1.1f, 0.0f));

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
}

void Player::UpdateRenderer()
{
	m_rendererObj->GetTransform()->SetLocalScale(!m_bIsHero || m_curCamMode ? glm::one<glm::vec3>() * 0.003f : glm::zero<glm::vec3>());

	const float rotationFactor = glm::min(glm::length(m_vVelocity) * glm::pi<float>() * 0.05f, 0.75f) * sin(m_fMoveTime * 1.75f);
	const float l = 60.0f;
	const float dx = l * sin(rotationFactor);
	const float dy = l * (1.0f - cos(rotationFactor));

	m_playerLookPitch = glm::mix(m_playerLookPitch, -m_cameraAngleAxis.x, DT * 16.0f);
	m_playerLookYaw = glm::mix(m_playerLookYaw, m_cameraAngleAxis.y, DT * 16.0f);
	m_rendererBodyAngleY = glm::clamp(m_rendererBodyAngleY, m_playerLookYaw - 30.0f, m_playerLookYaw + 30.0f);

	UpdateBodyRotation(m_rendererBodyAngleY);
	UpdateHeadRotation(m_playerLookPitch, m_playerLookYaw);

	m_transformLArm->SetLocalPosition(glm::vec3(dx, dy, m_transformLArm->GetLocalPosition().z));
	m_transformLArmOut->SetLocalPosition(glm::vec3(dx, dy, m_transformLArmOut->GetLocalPosition().z));
	m_transformRArm->SetLocalPosition(glm::vec3(-dx, dy, m_transformRArm->GetLocalPosition().z));
	m_transformRArmOut->SetLocalPosition(glm::vec3(-dx, dy, m_transformRArmOut->GetLocalPosition().z));
	m_transformLLeg->SetLocalPosition(glm::vec3(-dx, dy - 240.0f, m_transformLLeg->GetLocalPosition().z));
	m_transformLLegOut->SetLocalPosition(glm::vec3(-dx, dy - 240.0f, m_transformLLegOut->GetLocalPosition().z));
	m_transformRLeg->SetLocalPosition(glm::vec3(dx, dy - 240.0f, m_transformRLeg->GetLocalPosition().z));
	m_transformRLegOut->SetLocalPosition(glm::vec3(dx, dy - 240.0f, m_transformRLegOut->GetLocalPosition().z));

	m_transformLArm->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), glm::pi<float>(), rotationFactor)));
	m_transformLArmOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), glm::pi<float>(), rotationFactor)));
	m_transformRArm->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, -rotationFactor)));
	m_transformRArmOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, -rotationFactor)));
	m_transformLLeg->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, -rotationFactor)));
	m_transformLLegOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, -rotationFactor)));
	m_transformRLeg->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, rotationFactor)));
	m_transformRLegOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, rotationFactor)));
}

void Player::UpdateBodyRotation(float yaw)
{
	glm::quat bodyRotation = glm::quat(glm::vec3(0.0f, glm::radians(yaw - 90.0f), 0.0f));
	m_rendererObj->GetTransform()->SetLocalRotation(bodyRotation);
}

void Player::UpdateHeadRotation(float pitch, float yaw)
{
	glm::quat headRotation = glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(yaw - m_rendererBodyAngleY), 0.0f));
	headRotation = glm::rotate(headRotation, glm::radians(glm::clamp(pitch, -60.0f, 60.0f)), glm::vec3(0.0f, 1.0f, 0.0f));
	m_transformHead->SetLocalRotation(headRotation);
	if (m_transformHeadOut != nullptr)
		m_transformHeadOut->SetLocalRotation(headRotation);
}

shared_ptr<GameObj> Player::CreateParticlePrefab() const
{
	shared_ptr<GameObj> instance = make_obj<GameObj>();
	auto renderer = instance->AddComponent<MeshRenderer>();

	vector<Vertex> vertices;
	vector<GLuint> indices;
	for (int i = 0; i < 4; ++i)
	{
		Vertex v;
		v.position = glm::vec3(0.5f - i % 2, i / 2 - 0.5f, 0.0f);
		v.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		v.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
		v.uv = glm::vec2(i % 2, i / 2) * 0.25f + glm::one<glm::vec2>() * 0.75f;
		vertices.emplace_back(v);
	}
	indices = { 0, 1, 2, 1, 3, 2 };
	auto mesh = make_shared<Mesh>();
	mesh->Init(std::move(vertices), std::move(indices));
	shared_ptr<Model> model = make_shared<Model>();
	model->AddMesh(mesh);
	model->AddMaterial(make_shared<Material>());
	renderer->SetModelData(model);
	renderer->SetShader("DefaultShader.glsl");

	return instance;
}

void Player::HandleCollision() noexcept
{
	m_vVelocity = m_vVelocity + m_vAccelation * DT;

	glm::vec2 vVelocityXZ = glm::vec2(m_vVelocity.x, m_vVelocity.z);
	float l = glm::length(vVelocityXZ);
	if (l > m_vVelocityMax)
		vVelocityXZ *= m_vVelocityMax / l;
	if (l > 0.0f)
		vVelocityXZ = vVelocityXZ - glm::normalize(vVelocityXZ) * glm::min(l, 40.0f * DT);
	m_vVelocity.x = vVelocityXZ.x;
	m_vVelocity.z = vVelocityXZ.y;

	const glm::vec3 origin_pos = m_pCacheMyTransform->GetLocalPosition();

	const glm::vec3 position = origin_pos;
	glm::vec3 positionPost = position + m_vVelocity * DT;

	bool old_bGround = m_bGround;
	m_bGround = m_refTilemap->HandleCollision(position, positionPost, m_vVelocity);
	m_pCacheMyTransform->SetLocalPosition(positionPost);

	float old_fMoveTime = m_fMoveTime;
	m_fMoveTime += glm::length(m_vVelocity) * DT;

	bool bSoundPlay = false;
	if (m_bGround && glm::mod(old_fMoveTime, 2.5f) > glm::mod(m_fMoveTime, 2.5f))
		bSoundPlay = true;
	if (!old_bGround && m_bGround)
		bSoundPlay = true;

	if (bSoundPlay)
	{
		glm::vec3 vListenPos = m_pCacheMyTransform->GetWorldPosition();
		glm::vec3 vSoundPos = m_pCacheMyTransform->GetWorldPosition();
		float fDist = glm::distance(vListenPos, vSoundPos);
		if (fDist < 16.0f)
			Mgr(SoundMgr)->PlayEffect(std::format("grass{}.ogg", (int)(m_fMoveTime * 4) % 4 + 1), 0.5f * glm::pow(0.8f, fDist));
	}

	const glm::vec3 after_pos = m_pCacheMyTransform->GetLocalPositionAcc();

	m_bForceSendData |= !IsSameVector(origin_pos, after_pos);
}

void Player::UpdatePlayerCamFpsMode() noexcept
{
	const auto camTrans = m_cameraAnchor->GetTransform();
	glm::vec2 offset = Mgr(KeyMgr)->GetMouseDelta() * m_fCamSensivity;

	m_cameraAngleAxis += glm::vec3(offset.y, offset.x, 0.0f);
	m_cameraAngleAxis.x = glm::clamp(m_cameraAngleAxis.x, -89.0f, 89.0f);

	m_cameraAngleAxisSmooth = glm::mix(m_cameraAngleAxisSmooth, m_cameraAngleAxis, DT * 16.0f);
	camTrans->SetLocalRotation(glm::quat(glm::radians(m_cameraAngleAxisSmooth)));
}

void Player::UpdateCameraTransform(const shared_ptr<Transform>& pCameraTransfrom)noexcept
{
	glm::vec3 wv = m_cameraAnchor->GetTransform()->GetWorldPosition();
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
		pCameraTransfrom->SetLocalPosition(glm::vec3(0.0f, 0.0f, glm::max(target, glm::mix(pCameraTransfrom->GetLocalPosition().z, target, DT * 8.0f))));
		pCameraTransfrom->SetLocalRotation(glm::identity<glm::quat>());
	}
	break;
	case 2:
	{
		const RaycastResult result = m_refTilemap->RaycastTile(wv, this->GetPlayerLook(), fMaxDist);
		float target = result.hit ? glm::distance(wv, result.hitPosition) : fMaxDist;
		pCameraTransfrom->SetLocalPosition(glm::vec3(0.0f, 0.0f, glm::min(target, glm::mix(pCameraTransfrom->GetLocalPosition().z, target, DT * 8.0f))));
		pCameraTransfrom->SetLocalRotation(glm::quat(glm::vec3(0.0f, glm::pi<float>(), 0.0f)));
		break;
	}
	}
	float tParam = m_fMoveTime;
	float mParam = 0.04f;
	pCameraTransfrom->SetLocalPosition(glm::vec3(glm::sin(tParam) * mParam, glm::sin(tParam * 2.0f) * mParam, pCameraTransfrom->GetLocalPosition().z));
}

void Player::DestroyBlock(const RaycastResult& result)noexcept
{
	// 쓰고있는 텍스쳐의 이름 또는 인덱스, 아니면 텍스쳐포인터를 알면됌
	// Can get the texture index through the tilemap
	const int tileID = m_refTilemap->GetTile(result.hitTilePosition);
	if (tileID == 0)
		return;
	const int textureID = Tile::TEXTURES[tileID][2];
	m_refTilemap->SetTile(result.hitTilePosition, 0, true);

	const string tex_name = std::format("tile_{}.png", textureID);
	auto iter = g_mapParticleUniqueObject.find(tex_name);
	shared_ptr<Material> mate;
	// If the particle does not exist, create new particle object
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
	Mgr(ParticleMgr)->SetParticles(iter->second, 0.1f, (glm::vec3(result.hitTilePosition) + glm::one<glm::vec3>() * 0.5f));
	DestroyObj(iter->second);
	Mgr(SoundMgr)->PlayEffect(Tile::TILE_BREAK_SOUND[tileID], 0.5f);
}

void Player::CreateBlock(const RaycastResult& result,int tile_id)noexcept
{
	//const int tileID = Mgr(UIMgr)->GetSelectIndex() + 1;
	m_refTilemap->SetTile(result.hitTilePosition + glm::ivec3(result.hitNormal), tile_id, true);
	Mgr(SoundMgr)->PlayEffect(Tile::TILE_PLACE_SOUND[tile_id], 0.5f);
}

Player::Player(shared_ptr<MCTilemap> tilemap) : m_refTilemap(tilemap)
{
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
	m_cameraAnchor->GetTransform()->SetLocalPosition(glm::vec3(0.0f, 1.7f, 0.0f));

	m_cameraObj = make_obj<GameObj>();
	m_fpChangeCamMode[m_curCamMode]();

	m_pCamera = m_cameraObj->AddComponent<Camera>();
	m_pCamera->SetNear(1 / 64.0f);
	
	m_cursorBlockObj = CreateCursorBlockObj();
	m_cursorBlockObj->GetTransform()->SetLocalScale(glm::one<glm::vec3>() * (m_bIsHero ? 1.05f : 0.0f));

	m_particlePrefab = CreateParticlePrefab();

	m_vAccelation = glm::vec3(0.0f, -40.0f, 0.0f);
}

Player::~Player()
{

}

void Player::Start()
{
	InitializeRenderer();
	InitCamDirection();
	m_pCacheMyTransform = GetTransform();
	AddChild(m_rendererObj);
	AddChild(m_cameraAnchor);
	AddChild(m_cursorBlockObj);
	AddChild(make_shared<PlayerShadow>(m_refTilemap));
	m_cameraAnchor->AddChild(m_cameraObj);
	m_pCacheMyTransformCamera = m_cameraObj->GetTransform();

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
	if (!m_bIsHero)
	{
		m_vAccelation = glm::vec3(0.0f, -40.0f, 0.0f);
	}
	HandleCollision();
	UpdatePlayerCamFpsMode();
	UpdateCameraTransform(m_pCacheMyTransformCamera);
	GameObj::Update();

	m_pCamera->SetCamFov(Lerp(m_pCamera->GetCamFov(), glm::radians(KEY_HOLD(GLFW_KEY_LEFT_CONTROL) ? 60.0f : 45.0f), DT * 8.0f));
	UpdateRenderer();
}

glm::vec3 Player::GetPlayerLook() const noexcept
{
	return glm::rotate(glm::quat(glm::vec3(glm::radians(m_cameraAngleAxisSmooth.x), glm::radians(m_cameraAngleAxisSmooth.y), 0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Player::Fire() noexcept
{
	auto bullet = make_shared<Bullet>(GetTransform()->GetWorldPosition(), GetPlayerLook());
	CreateObj(std::move(bullet), GROUP_TYPE::PROJ_PLAYER);
}