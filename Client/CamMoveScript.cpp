#include "pch.h"
#include "CamMoveScript.h"
#include "Transform.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "Camera.h"
#include "GameObj.h"

CamMoveScript::CamMoveScript()
	:MonoBehavior{"CamMoveScript"}
{
}

CamMoveScript::~CamMoveScript()
{
}

void CamMoveScript::Awake()
{
	assert(GetGameObj()->GetComp<Camera>());
}

void CamMoveScript::Update()
{
	const auto camTrans = GetTransform();
	if (KEY_TAP(GLFW_KEY_TAB))
	{
		if (!m_bIsFixed)
		{
			SetLookSlerp(m_vLookPoint);
		}
		SetFixed(!m_bIsFixed);
	}

	if (!m_bIsFixed)
	{
		if (KEY_HOLD(GLFW_KEY_A))
		{
			camTrans->AddLeftRight(-m_fMoveSpeed * DT);
		}
		if (KEY_HOLD(GLFW_KEY_W))
		{
			camTrans->AddFrontBack(m_fMoveSpeed * DT);
		}
		if (KEY_HOLD(GLFW_KEY_S))
		{
			camTrans->AddFrontBack(-m_fMoveSpeed * DT);
		}
		if (KEY_HOLD(GLFW_KEY_D))
		{
			camTrans->AddLeftRight(m_fMoveSpeed * DT);
		}
		if (KEY_HOLD(GLFW_KEY_Q))
		{
			camTrans->AddWorldRotation(-50.f * DT, Y_AXIS);
		}
		if (KEY_HOLD(GLFW_KEY_E))
		{
			camTrans->AddWorldRotation(50.f * DT, Y_AXIS);
		}
		if (KEY_HOLD(GLFW_KEY_UP))
		{
			camTrans->AddUpDown(m_fMoveSpeed * DT);
		}
		if (KEY_HOLD(GLFW_KEY_DOWN))
		{
			camTrans->AddUpDown(-m_fMoveSpeed * DT);
		}
	}
}

void CamMoveScript::LateUpdate()
{
	if (!m_bIsFixed)
	{
		const auto camTrans = GetTransform();

		const glm::vec3 camPos = camTrans->GetWorldPositionRecursion();
		const glm::vec3 camLook = camTrans->GetLookRecursion();

		if (!IsFloatZero(camLook.z))
		{
			m_vLookPoint.x = camLook.x * -camPos.z / camLook.z + camPos.x;
			m_vLookPoint.y = camLook.y * -camPos.z / camLook.z + camPos.y;
			m_vLookPoint.z = 0.f;
		}
		else
		{
			if (!IsFloatZero(camLook.x))
			{
				m_vLookPoint.x = camLook.x * -camPos.y / camLook.y + camPos.x;
				m_vLookPoint.y = 0.f;
				m_vLookPoint.z = camLook.z * -camPos.y / camLook.y + camPos.z;
			}
			else
			{
				m_vLookPoint.x = 0.f;
				m_vLookPoint.y = camLook.y * -camPos.x / camLook.x + camPos.y;
				m_vLookPoint.z = camLook.z * -camPos.x / camLook.x + camPos.z;
			}
		}
	}
}

void CamMoveScript::LastUpdate()
{
	const auto camTrans = GetTransform();

	if (m_bIsCamMoveForLook)
	{
		m_fSlerpRatio = glm::fclamp(m_fSlerpRatio + m_fSlerpSpeed * DT, 0.f, 1.f);
		m_forUpdateQuat = glm::slerp(m_startQuat, m_endQuat, m_fSlerpRatio);

		if (1.f == m_fSlerpRatio)
		{
			m_fSlerpRatio = 0.f;
			m_bIsCamMoveForLook = false;
		}
	}
	else
	{
		if (m_bIsFixed)
		{
			const glm::vec3 direction = glm::normalize(m_vLookPoint - camTrans->GetWorldPositionRecursion());
			//m_forUpdateQuat = glm::quatLookAtLH(direction, camTrans->GetUpRecursion());
			m_forUpdateQuat = glm::quatLookAtLH(direction, Y_AXIS);
		}
		else
		{
			const glm::vec2 offset = Mgr(KeyMgr)->GetMouseDelta() * m_fCamSensivity;
			const glm::quat camQuat = camTrans->GetWorldRotationRecursion();
			
			const glm::quat pitch = glm::angleAxis(glm::radians(offset.y), GetRightByQuat(camQuat));
			//const glm::quat yaw = glm::angleAxis(glm::radians(offset.x), GetUpByQuat(camQuat));
			const glm::quat yaw = glm::angleAxis(glm::radians(offset.x), Y_AXIS);
			const glm::vec3 newLookVec = yaw * pitch * GetLookByQuat(camQuat);

			//m_forUpdateQuat = glm::quatLookAtLH(glm::normalize(newLookVec), GetUpByQuat(camQuat));
			m_forUpdateQuat = glm::quatLookAtLH(glm::normalize(newLookVec), Y_AXIS);
		}
	}

	camTrans->SetLocalRotation(m_forUpdateQuat);
}

void CamMoveScript::SetLookSlerp(const glm::vec3& _LookPoint, const float _fSpeed)
{
	if (m_bIsCamMoveForLook)
	{
		return;
	}

	const auto camTrans = GetTransform();

	const glm::vec3 curPos = camTrans->GetWorldPositionRecursion();

	m_startQuat = glm::quatLookAtLH(glm::normalize(m_vLookPoint - curPos), GetUpByQuat(m_forUpdateQuat));
	m_vLookPoint = _LookPoint;
	m_endQuat = glm::quatLookAtLH(glm::normalize(m_vLookPoint - curPos), GetUpByQuat(m_forUpdateQuat));
	m_fSlerpSpeed = _fSpeed;
	m_bIsCamMoveForLook = true;
}