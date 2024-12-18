#include "pch.h"
#include "Mesh.h"
#include "Hero.h"
#include "Model.h"
#include "KeyMgr.h"
#include "MCTilemap.h"
#include "Transform.h"
#include "TimeMgr.h"
#include "Camera.h"
#include "PlayerShadow.h"
#include "AssimpMgr.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "SoundMgr.h"
#include "Monster.h"
#include "UIMgr.h"
#include "PacketBase.hpp"
#include "NetworkMgr.h"
#include "ProjectileArrow.h"
#include "ProjectileFireball.h"
#include "EventMgr.h"
#include "MCItemManager.h"
#include "SceneMgr.h"
#include "Scene.h"

extern std::atomic_bool g_bTileFinish;

static bool RayAABBIntersection(const glm::vec3 rayOrigin, const glm::vec3 rayDir, const glm::vec3 aabbMin, const glm::vec3 aabbMax, float& t)
{
	glm::vec3 invDir = 1.0f / rayDir;
	glm::vec3 tMin = (aabbMin - rayOrigin) * invDir;
	glm::vec3 tMax = (aabbMax - rayOrigin) * invDir;

	glm::vec3 t1 = glm::min(tMin, tMax);
	glm::vec3 t2 = glm::max(tMin, tMax);

	float tNear = glm::max(glm::max(t1.x, t1.y), t1.z);
	float tFar = glm::min(glm::min(t2.x, t2.y), t2.z);

	if (tNear > tFar || tFar < 0.0f)
		return false;

	t = tNear;
	return true;
}

Hero::Hero(std::shared_ptr<MCTilemap> pTilemap) noexcept
	:Player{ std::move(pTilemap) }
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

	m_bIsHero = true;
	m_pCamera->SetMainCam();

	m_inventory[0] = MCItemStack{ Mgr(MCItemManager)->GetItemByID(0), 64 };
	m_inventory[1] = MCItemStack{ Mgr(MCItemManager)->GetItemByID(1), 64 };
	m_inventory[2] = MCItemStack{ Mgr(MCItemManager)->GetItemByID(2), 64 };
	m_inventory[3] = MCItemStack{ Mgr(MCItemManager)->GetItemByID(3), 64 };
	m_inventory[4] = MCItemStack{ Mgr(MCItemManager)->GetItemByID(4), 64 };
	m_inventory[5] = MCItemStack{ Mgr(MCItemManager)->GetItemByID(5), 64 };
	m_inventory[6] = MCItemStack{ Mgr(MCItemManager)->GetItemByID(6), 0 };
	m_inventory[7] = MCItemStack{ Mgr(MCItemManager)->GetItemByID(7), 1 };
	m_inventory[8] = MCItemStack{ Mgr(MCItemManager)->GetItemByID(8), 0 };
	UpdatePlayerInventoryUI();
}

void Hero::Start()
{
	Player::Start();

	InitCamDirection();

	AddChild(m_cameraAnchor);
	AddChild(m_cursorBlockObj);

	m_cameraAnchor->AddChild(m_cameraObj);
	m_pCacheMyTransformCamera = m_cameraObj->GetTransform();

	SetPlayerControl(true);
	Mgr(UIMgr)->SetPopdownCallback([this]() { SetPlayerControl(true); });
}

shared_ptr<GameObj> Hero::CreateCursorBlockObj() const
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

shared_ptr<GameObj> Hero::CreateParticlePrefab() const
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

void Hero::InitCamDirection() noexcept
{
	m_cameraAngleAxis = glm::zero<glm::vec3>();
	m_cameraAngleAxisSmooth = glm::zero<glm::vec3>();
	m_cameraAnchor->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
	m_rendererObj->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
	m_pCamera->GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
	GetTransform()->SetLocalRotation(glm::identity<glm::quat>());
}

void Hero::UpdatePlayerCamFpsMode() noexcept
{
	const auto camTrans = m_cameraAnchor->GetTransform();
	glm::vec2 offset = Mgr(KeyMgr)->GetMouseDelta() * m_fCamSensivity;

	m_cameraAngleAxis += glm::vec3(offset.y, offset.x, 0.0f);
	m_cameraAngleAxis.x = glm::clamp(m_cameraAngleAxis.x, -89.0f, 89.0f);

	m_cameraAngleAxisSmooth = glm::mix(m_cameraAngleAxisSmooth, m_cameraAngleAxis, DT * 16.0f);
	camTrans->SetLocalRotation(glm::quat(glm::radians(m_cameraAngleAxisSmooth)));
}

void Hero::UpdateCameraTransform(const shared_ptr<Transform>& pCameraTransfrom)noexcept
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
		const RaycastResult result = m_refTilemap->RaycastTile(wv, -this->GetCameraDirection(), fMaxDist);
		float target = -(result.hit ? glm::distance(wv, result.hitPosition) : fMaxDist);
		pCameraTransfrom->SetLocalPosition(glm::vec3(0.0f, 0.0f, glm::max(target, glm::mix(pCameraTransfrom->GetLocalPosition().z, target, DT * 8.0f))));
		pCameraTransfrom->SetLocalRotation(glm::identity<glm::quat>());
	}
	break;
	case 2:
	{
		const RaycastResult result = m_refTilemap->RaycastTile(wv, this->GetCameraDirection(), fMaxDist);
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

glm::vec3 Hero::GetCameraDirection() const
{
	return glm::rotate(glm::quat(glm::vec3(glm::radians(m_cameraAngleAxisSmooth.x), glm::radians(m_cameraAngleAxisSmooth.y), 0.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Hero::SetPlayerControl(bool bControl) noexcept
{
	m_bIsPlayerControl = bControl;

	Mgr(KeyMgr)->SetMouseMode(bControl ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Hero::UpdatePlayerInventoryUI() noexcept
{
	for (int i = 0; i < 9; ++i)
	{
		string texName = m_inventory[i].GetItem()->GetIconTexture();
		pair<string_view, int> p = make_pair(string_view(texName), m_inventory[i].GetStackSize());
		Mgr(UIMgr)->UpdateInventoryUI(i, p);
	}
}

void Hero::InputMove() noexcept
{
	if (KEY_HOLD(GLFW_KEY_A))
	{
		MoveByView(glm::vec3(-1.0f, 0.0f, 0.0f) * 100.0f);
	}
	if (KEY_HOLD(GLFW_KEY_W))
	{
		MoveByView(glm::vec3(0.0f, 0.0f, 1.0f) * 100.0f);
	}
	if (KEY_HOLD(GLFW_KEY_S))
	{
		MoveByView(glm::vec3(0.0f, 0.0f, -1.0f) * 100.0f);
	}
	if (KEY_HOLD(GLFW_KEY_D))
	{
		MoveByView(glm::vec3(1.0f, 0.0f, 0.0f) * 100.0f);
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
			m_vVelocity += glm::vec3(0.0f, 10.0f, 0.0f);
		// MoveByView(glm::vec3(0.0f, 1.0f, 0.0f) * 10.0f);
		m_bForceSendData = true;
	}
	if (KEY_HOLD(GLFW_KEY_LEFT_SHIFT))
	{
		MoveByView(glm::vec3(0.0f, -1.0f, 0.0f) * 100.0f);
		m_bForceSendData = true;
	}
	if (KEY_TAP(GLFW_KEY_F5))
	{
		m_curCamMode = wrapAround(m_curCamMode + 1, 0, 3);
		m_fpChangeCamMode[m_curCamMode]();
	}
}

void Hero::UpdateTileManipulation()noexcept
{
	//if (!g_bTileFinish)
	//{
	//	return;
	//}

	const glm::vec3 wv = m_cameraAnchor->GetTransform()->GetWorldPosition();
	const RaycastResult result = m_refTilemap->RaycastTile(wv, this->GetCameraDirection(), 10.0f);
	m_cursorBlockObj->GetTransform()->SetLocalPosition((glm::vec3(result.hitTilePosition) + glm::one<glm::vec3>() * 0.5f) - GetTransform()->GetLocalPosition());

	if (KEY_TAP(GLFW_MOUSE_BUTTON_LEFT))
	{
		bool monsterHit = false;
		for (const auto& monster: Mgr(SceneMgr)->GetCurScene()->GetGroupObj(GROUP_TYPE::MONSTER))
		{
			const glm::vec3 monsterPos = monster->GetTransform()->GetWorldPosition();
			float width = 0.5f;
			float height = 3.0f;
			float t;
			if (RayAABBIntersection(wv, GetCameraDirection(), monsterPos - glm::vec3(width, 0, width), monsterPos + glm::vec3(width, height, width), t) && t < 5.0f)
			{
				if (const auto val_mon = dynamic_cast<ServerObject*>(monster.get()))
				{
					monsterHit = true;
					c2s_HIT_MONSTER pkt;
					pkt.hit_monster_id = val_mon->GetID();
					Send(pkt);
					break;
				}
			}
		}

		if (0 == result.hitTilePosition.y)return;
		if (!monsterHit && result.hit)
		{
			c2s_DESTROY_BLOCK pkt;
			pkt.x = result.hitTilePosition.x;
			pkt.y = result.hitTilePosition.y;
			pkt.z = result.hitTilePosition.z;
			Mgr(NetworkMgr)->Send(pkt);
		}

	}
	if (KEY_TAP(GLFW_MOUSE_BUTTON_RIGHT))
	{
		int index = Mgr(UIMgr)->GetSelectIndex();
		MCItemStack& stack = m_inventory[index];
		if (stack.GetStackSize() <= 0)
			return;
		if (m_inventory[index].GetItem()->OnUseItem(m_refTilemap.get(), this, result))
		{
			MCItemStack::AddStack(stack, -1);
			UpdatePlayerInventoryUI();
		}
	}
}

void Hero::Update()
{
	m_rendererObj->GetTransform()->SetLocalScale(!m_bIsHero || m_curCamMode ? glm::one<glm::vec3>() * 0.003f : glm::zero<glm::vec3>());

	m_vAccelation = glm::vec3(0.0f, -40.0f, 0.0f);
	
	if (m_bIsPlayerControl)
	{
		InputMove();
		UpdatePlayerCamFpsMode();
		UpdateTileManipulation();
	}

	m_lookYaw = glm::mix(m_lookYaw, m_cameraAngleAxis.y, DT * 16.0f);
	m_lookPitch = glm::mix(m_lookPitch, -m_cameraAngleAxis.x, DT * 16.0f);

	Player::Update();
	m_fAccTime += DT;
	if (G_SEND_INTERVAL <= m_fAccTime || m_bForceSendData)
	{
		SendMyMoveData();
		m_fAccTime = 0.f;
		m_bForceSendData = false;
	}

	m_pCamera->SetCamFov(Lerp(m_pCamera->GetCamFov(), glm::radians(KEY_HOLD(GLFW_KEY_LEFT_CONTROL) ? 60.0f : 45.0f), DT * 8.0f));
	UpdateCameraTransform(m_pCacheMyTransformCamera);
}

void Hero::AddItemToInventory(const MCItemStack& item) noexcept
{
	for (int i = 0; i < 9; ++i)
	{
		if (m_inventory[i].GetItem() == item.GetItem())
		{
			MCItemStack::AddStack(m_inventory[i], item.GetStackSize());
			UpdatePlayerInventoryUI();
			return;
		}
	}
}

void Hero::OnObjectDamaged(int value)
{
	Mgr(UIMgr)->SetHealth(value);

	// play Player_hurt1.ogg
	Mgr(SoundMgr)->PlayEffect("Player_hurt1.ogg", 1.0f);
	m_cameraAngleAxisSmooth.z = 30.0f;
	float theta = (m_lookYaw + 90.0f) * F_DEG2RAD;
	float magnitude = 10.0f;
	m_vVelocity += glm::vec3(glm::cos(theta), 1.0f, glm::sin(theta)) * magnitude;

	Player::OnObjectDamaged(value);
}

void Hero::OnObjectDead()
{
	Player::OnObjectDead();

	SetPlayerControl(false);
	Mgr(UIMgr)->SetGameOverPanelActive(true);
	Mgr(UIMgr)->SetHealth(PLAYER_START_HP);
}

void Hero::SendMyMoveData() const noexcept
{
	c2s_MOVE_OBJECT pkt;
	const auto pos = m_pCacheMyTransform->GetWorldPosition();
	pkt.position_x = pos.x;
	pkt.position_y = pos.y;
	pkt.position_z = pos.z;

	pkt.rotation_y = m_rendererBodyAngleY;
	pkt.yaw = m_lookYaw;
	pkt.pitch = m_lookPitch;

	pkt.velocity_x = m_vVelocity.x;
	pkt.velocity_y = m_vVelocity.y;
	pkt.velocity_z = m_vVelocity.z;

	pkt.acceleration_x = m_vAccelation.x;
	pkt.acceleration_y = m_vAccelation.y;
	pkt.acceleration_z = m_vAccelation.z;

	pkt.cam_x = m_cameraAngleAxisSmooth.x;
	pkt.cam_y = m_cameraAngleAxisSmooth.y;
	pkt.cam_z = m_cameraAngleAxisSmooth.z;

	Send(pkt);
}

void Hero::MoveByView(const glm::vec3& vDelta)
{
	glm::vec3 vWorldDelta = glm::quat(glm::vec3(0.0f, glm::radians(m_cameraAngleAxis.y), 0.0f)) * vDelta;
	m_vAccelation += vWorldDelta;
	m_rendererBodyAngleY = Lerp(m_rendererBodyAngleY, m_cameraAngleAxis.y, DT * 8.0f);
	m_bForceSendData = true;
}

void Hero::Fire()
{
	const glm::vec3 r = GetCameraDirection();
	const glm::vec3 p = GetPosition() + glm::vec3(0.0f, 1.7f, 0.0f) + glm::normalize(r) * 0.75f;

	c2s_ADD_PROJECTILE pkt;

	pkt.pos_x = p.x;
	pkt.pos_y = p.y;
	pkt.pos_z = p.z;

	pkt.dir_x = m_cameraAngleAxisSmooth.x;
	pkt.dir_y = m_cameraAngleAxisSmooth.y;

	Send(pkt);
}