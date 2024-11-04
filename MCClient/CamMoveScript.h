#pragma once
#include "MonoBehavior.h"

class CamMoveScript
	:public MonoBehavior
{
	COMP_CLONE(CamMoveScript)
private:
	glm::vec3 m_vLookPoint = ZERO_VEC3;

	float m_fSlerpRatio = 0.f;
	float m_fSlerpSpeed = 0.f;
	bool m_bIsCamMoveForLook = false;

	glm::quat m_forUpdateQuat = {};
	glm::quat m_startQuat = {};
	glm::quat m_endQuat = {};

	bool m_bIsFixed = true;
	float m_fCamSensivity = 0.f;

	float m_fMoveSpeed = 10.f;
public:
	CamMoveScript();
	~CamMoveScript();

	void Awake()override;
	void Update()override;

	void LateUpdate()override;
	void LastUpdate()override;

	void SetFixed(bool _b){ 
		m_bIsFixed = _b;
		m_bIsFixed ? m_fCamSensivity = 0.f : m_fCamSensivity = 0.03f;
	}
	bool GetIsFixed()const { return m_bIsFixed; }
	void SetLookSlerp(const glm::vec3& _LookPoint, const float _fSpeed = 2.f);
	void SetLookAtNow(const glm::vec3& _LookPoint) { m_vLookPoint = _LookPoint; }
	const glm::vec3& GetLookPoint()const { return m_vLookPoint; }

	void SetMoveSpeed(const float _fSpeed) { m_fMoveSpeed = _fSpeed; }
	float GetMoveSpeed()const { return m_fMoveSpeed; }

	void SetCamMouseSensivity(const float _fSensivity) { m_fCamSensivity = _fSensivity; }
	float GetCamMouseSensivity()const { return m_fCamSensivity; }
};

