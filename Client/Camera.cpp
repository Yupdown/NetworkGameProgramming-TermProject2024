#include "pch.h"
#include "Camera.h"
#include "Transform.h"
#include "Core.h"
#include "TimeMgr.h"
#include "KeyMgr.h"

GLint Camera::g_curShaderID;
shared_ptr<Camera> Camera::g_curMainCam;
shared_ptr<Camera> Camera::g_observer;
glm::mat4 Camera::g_mat4MainView = glm::mat4{ 1.f };
glm::mat4 Camera::g_mat4Ortho;

shared_ptr<Camera> Camera::GetCurCam(){return g_curMainCam;}

void Camera::SetMainCam(shared_ptr<Camera> _pTargetCam)
{
	_pTargetCam->SetMainCam();
}

Camera::Camera()
	:Component{COMPONENT_TYPE::CAMERA}
{
}

Camera::~Camera()
{
}

void Camera::FinalUpdate()
{
	if (m_bShakeFlag)
	{
		return;
	}
	const auto camTrans = GetTransform();

	/*if (m_bIsMainCam)
	{
		camTrans->GetWaitLock().Wait();
	}*/
	m_matViewInv = camTrans->GetLocalToWorldMatrix();
	m_matView = glm::inverse(m_matViewInv);

	if (m_bIsMainCam)
	{
		g_mat4MainView = m_matView;
	}

	if (m_bIsChanging)
	{
		return;
	}

	const auto [width, height] = Mgr(Core)->GetWidthHeight();

	if (m_eProjType == PROJECTION_TYPE::PERSPECTIVE)
	{
		m_matProjection = glm::perspectiveFovLH(m_fov, width, height, m_near, m_far);
	}
	else
	{
		g_mat4Ortho = m_matProjection = glm::orthoLH(-width / 2.0f * m_camOrthoFactor, width / 2.0f * m_camOrthoFactor, -height / 2.0f * m_camOrthoFactor, height / 2.0f * m_camOrthoFactor, -10000.f, 10000.f);
	}
	m_matProjInv = glm::inverse(m_matProjection);
}

void Camera::ShakeCam(const float fShakeIntensity_, const float fDuration_) noexcept
{
	m_bShakeFlag = true;
	StartCoRoutine("ShakeCam", [](bool& flag ,const float fShakeIntensity_, const float fDuration_,shared_ptr<Camera> pCam)noexcept->CoRoutine {
		for (float acc = 0.f; acc < fDuration_; acc += DT)
		{
			pCam->m_matView = glm::translate(pCam->m_matView, glm::sphericalRand(fShakeIntensity_));
			co_await std::suspend_always{};
		}
		flag = false;
		co_return;
		}(m_bShakeFlag,fShakeIntensity_,fDuration_,shared_from_this()));
}

void Camera::PreRender() const
{
	//glGetIntegerv(GL_CURRENT_PROGRAM, &g_curShaderID);
	//const glm::mat4 viewMat = m_eProjType == PROJECTION_TYPE::PERSPECTIVE ? g_mat4MainView : glm::mat4{ 1.f };
	//glUniformMatrix4fv(glGetUniformLocation(g_curShaderID, "uView"), 1, GL_FALSE, glm::value_ptr(viewMat));
	//glUniformMatrix4fv(glGetUniformLocation(g_curShaderID, "uProj"), 1, GL_FALSE, glm::value_ptr(m_matProjection));
	//glUniform3fv(glGetUniformLocation(g_curShaderID, "uViewPos"), 1, glm::value_ptr(g_curMainCam->GetTransform()->GetWorldPosition()));
	
	//static const GLuint uboIdx = Mgr(Core)->GetUBO_ID();
	//glBindBuffer(GL_UNIFORM_BUFFER, uboIdx);
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &m_matProjection);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::StartChangeCamProjType() noexcept
{
	const auto [width, height] = Mgr(Core)->GetWidthHeight();
	m_bIsChanging = true;
	m_startMat = m_matProjection;
	m_endMat = PROJECTION_TYPE::PERSPECTIVE == m_eProjType ? glm::orthoLH(-width / 2.0f * m_camOrthoFactor, width / 2.0f * m_camOrthoFactor, -height / 2.0f * m_camOrthoFactor, height / 2.0f * m_camOrthoFactor, -10000.f, 10000.f):
		glm::perspectiveFovLH(m_fov, width, height, m_near, m_far);
	
	StartCoRoutine("ChangeCamProj", UpdateChangeProj());
}

CoRoutine Camera::UpdateChangeProj() noexcept
{
	for (m_camInterpolation; m_camInterpolation <= 1.f; m_camInterpolation += DT)
	{
		float t = (1.0f - glm::cos(m_camInterpolation * glm::pi<float>())) * 0.5f;
		m_matProjection = glm::mix(m_startMat, m_endMat, t);
		co_await std::suspend_always{};
	}
	m_eProjType = PROJECTION_TYPE::PERSPECTIVE == m_eProjType ? PROJECTION_TYPE::ORTHOGRAPHIC : PROJECTION_TYPE::PERSPECTIVE;
	m_bIsChanging = false;
	m_camInterpolation = 0.f;
	co_return;
}

