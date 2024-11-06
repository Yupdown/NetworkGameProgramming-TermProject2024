#include "pch.h"
#include "Hero.h"
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
	m_bIsHero = true;
	m_pCamera->SetMainCam();
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
	const RaycastResult result = m_refTilemap->RaycastTile(wv, this->GetPlayerLook(), 10.0f);
	m_cursorBlockObj->GetTransform()->SetLocalPosition((glm::vec3(result.hitTilePosition) + glm::one<glm::vec3>() * 0.5f) - GetTransform()->GetLocalPosition());

	if (KEY_TAP(GLFW_MOUSE_BUTTON_LEFT) && result.hit)
	{
		m_refTilemap->SetTile(result.hitTilePosition, 0, true);
	}
	if (KEY_TAP(GLFW_MOUSE_BUTTON_RIGHT) && result.hit)
	{
		const int tileID = Mgr(UIMgr)->GetSelectIndex() + 1;
		m_refTilemap->SetTile(result.hitTilePosition + glm::ivec3(result.hitNormal), tileID, true);
	}
}

void Hero::Update()
{
	m_vAccelation = glm::vec3(0.0f, -40.0f, 0.0f);
	InputMove();
	UpdateTileManipulation();
	Player::Update();
	m_fAccTime += DT;
	if (0.01f <= m_fAccTime || m_bForceSendData)
	{
		m_fAccTime = 0.f;
		m_bForceSendData = false;
	}
}

void Hero::OnDamaged()
{
	// play Player_hurt1.ogg
	Mgr(SoundMgr)->PlayEffect("Player_hurt1.ogg", 1.0f);
	m_cameraAngleAxisSmooth.z = 30.0f;
	float theta = (m_playerLookYaw + 90.0f) * F_DEG2RAD;
	float magnitude = 10.0f;
	m_vVelocity += glm::vec3(glm::cos(theta), 1.0f, glm::sin(theta)) * magnitude;
}

void Hero::MoveByView(const glm::vec3& vDelta)
{
	glm::vec3 vWorldDelta = glm::quat(glm::vec3(0.0f, glm::radians(m_cameraAngleAxis.y), 0.0f)) * vDelta;
	m_vAccelation += vWorldDelta;
	m_rendererBodyAngleY = Lerp(m_rendererBodyAngleY, m_cameraAngleAxis.y, DT * 8.0f);
	m_bForceSendData = true;
}