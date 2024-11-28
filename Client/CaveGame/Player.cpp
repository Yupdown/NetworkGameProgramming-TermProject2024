#include "pch.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "Player.h"
#include "PlayerCam.h"
#include "AssimpMgr.h"
#include "Transform.h"
#include "Camera.h"
#include "AssimpMgr.h"
#include "ProjectileArrow.h"
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
#include "NetworkMgr.h"
#include "ServerObjectManager.h"

extern std::atomic_bool g_bTileFinish;
static std::uniform_int_distribution tile_num{ 1,12 };

void Player::SetRendererTexture(int id)
{
	m_rendererTextureId = id;

	ValidateRendererTexture();
}

void Player::ValidateRendererTexture()
{
	std::string tex_name = std::format("char_{:02}.png", m_rendererTextureId % G_NUM_OF_PLAYER_MODEL + 1);

	// replace all materials with the same material
	for (auto& child : *m_rendererObj)
	{
		auto meshRenderer = child->GetComp<MeshRenderer>();
		if (meshRenderer == nullptr)
			continue;
		for (auto& m : meshRenderer->GetMaterial())
			m->AddTexture2D(tex_name);
	}
}

void Player::InitializeRenderer()
{
	m_rendererObj = Mgr(AssimpMgr)->LoadAllPartsAsGameObj("DefaultWarpShader.glsl", "Player.fbx");
	m_rendererObj->GetTransform()->SetLocalPosition(glm::vec3(0.0f, 1.1f, 0.0f));
	m_rendererObj->GetTransform()->SetLocalScale(0.003f);

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

	ValidateRendererTexture();
}

void Player::UpdateRenderer()
{
	const float rotationFactor = glm::min(glm::length(m_vVelocity) * glm::pi<float>() * 0.05f, 0.75f) * sin(m_fMoveTime * 1.75f);
	const float l = 60.0f;
	const float dx = l * sin(rotationFactor);
	const float dy = l * (1.0f - cos(rotationFactor));
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

	m_transformLArm->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, rotationFactor)));
	m_transformLArmOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), 0.0f, rotationFactor)));
	m_transformRArm->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), glm::pi<float>(), -rotationFactor)));
	m_transformRArmOut->SetLocalRotation(glm::quat(glm::vec3(glm::radians(90.0f), glm::pi<float>(), -rotationFactor)));
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

void Player::DestroyBlock(const glm::ivec3& hitTilePosition)noexcept
{
	// 쓰고있는 텍스쳐의 이름 또는 인덱스, 아니면 텍스쳐포인터를 알면됌
	// Can get the texture index through the tilemap
	const uint8_t tileID = m_refTilemap->GetTile(hitTilePosition);
	if (tileID == 0)
		return;
	const int textureID = Tile::TEXTURES[tileID][2];
	m_refTilemap->SetTile(hitTilePosition, 0, true);

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
	Mgr(ParticleMgr)->SetParticles(iter->second, 0.1f, (glm::vec3(hitTilePosition) + glm::one<glm::vec3>() * 0.5f));
	DestroyObj(iter->second);
	Mgr(SoundMgr)->PlayEffect(Tile::TILE_BREAK_SOUND[tileID], 0.5f);
}

void Player::CreateBlock(const glm::ivec3& hitTilePosition, uint8_t tile_id)noexcept
{
	//const int tileID = Mgr(UIMgr)->GetSelectIndex() + 1;
	m_refTilemap->SetTile(hitTilePosition, tile_id, true);
	Mgr(SoundMgr)->PlayEffect(Tile::TILE_PLACE_SOUND[tile_id], 0.5f);
}

Player::Player(shared_ptr<MCTilemap> tilemap) : m_refTilemap(tilemap)
{
	m_vAccelation = glm::vec3(0.0f, -40.0f, 0.0f);
}

Player::~Player()
{

}

void Player::Start()
{
	InitializeRenderer();
	m_pCacheMyTransform = GetTransform();
	AddChild(m_rendererObj);
	AddChild(make_shared<PlayerShadow>(m_refTilemap));

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
		m_rendererBodyAngleY = m_lookYaw; // TODO : 이거 이대로 해도 ㄱㅊ?
		m_playerLookYaw = m_lookYaw;
		m_playerLookPitch = m_lookPitch;
	}

	HandleCollision();
	UpdateRenderer();
	// UpdatePlayerCamFpsMode();

	ServerObject::Update();
}

class ProjectileArrow* Player::Fire(const glm::vec3& arrow_pos, const float x_, const float y_) noexcept
{
	auto projectile = make_shared<ProjectileArrow>(m_refTilemap);

	const glm::vec3 p = arrow_pos + glm::vec3(0.0f, 1.7f, 0.0f);

	const auto r = glm::rotate(glm::quat(glm::vec3(glm::radians(x_), glm::radians(y_), 0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));

	projectile->SetPosition(p);
	projectile->SetVelocity(r * 32.0f); // GetPlayerLook 이였음
	
	if (m_bIsHero)
	{
		c2s_ADD_PROJECTILE pkt;
		pkt.pos_x = arrow_pos.x;
		pkt.pos_y = arrow_pos.y;
		pkt.pos_z = arrow_pos.z;

		pkt.dir_x = x_;
		pkt.dir_y = y_;

		pkt.local_arrow_id = projectile->GetArrowLocalID();

		Send(pkt);
	}

	const auto temp = projectile.get();

	CreateObj(std::move(projectile), GROUP_TYPE::PROJ_PLAYER);

	return temp;
}