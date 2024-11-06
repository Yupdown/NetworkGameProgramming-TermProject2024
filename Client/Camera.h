#pragma once
#include "Component.h"

class Camera
	:public Component
	,public enable_shared_from_this<Camera>
{
	 COMP_CLONE(Camera)
	 static GLint g_curShaderID;
	 static shared_ptr<Camera> g_curMainCam;
	 static shared_ptr<Camera> g_observer;
	 static glm::mat4 g_mat4MainView;
	 static glm::mat4 g_mat4Ortho;
private:
	PROJECTION_TYPE m_eProjType = PROJECTION_TYPE::PERSPECTIVE;

	float m_near = 0.1f;
	float m_far = 1000.0f;
	float m_fov = glm::pi<float>() / 4.f;
	float m_scale = 1.f;

	glm::mat4 m_matView = glm::mat4{ 1.f };
	glm::mat4 m_matProjection = glm::mat4{ 1.f };

	glm::mat4 m_startMat = glm::mat4{ 1.f };
	glm::mat4 m_endMat = glm::mat4{ 1.f };

	glm::mat4 m_matViewInv = glm::mat4{ 1.f };
	glm::mat4 m_matProjInv = glm::mat4{ 1.f };

	bool	m_bIsMainCam = false;
	float m_camOrthoFactor = 0.005f;
	float m_camInterpolation = 0.f;
	bool m_bIsChanging = false;
	bool m_bShakeFlag = false;
private:
	CoRoutine UpdateChangeProj()noexcept;
public:
	static shared_ptr<Camera> GetCurCam();
	static glm::mat4 GetOrthoProjMat() noexcept { return g_mat4Ortho; }
	static glm::mat4 GetMainCamViewMat()noexcept { return g_mat4MainView; }
	static void SetMainCam(shared_ptr<Camera> _pTargetCam);
	static shared_ptr<Camera> GetObserverCam()noexcept { return g_observer; }
	static const std::pair<glm::mat4, glm::mat4> GetMainCamProjViewMat()noexcept { return g_curMainCam->GetCamMatProjView(); }
	static const std::pair<glm::mat4, glm::mat4> GetMainCamProjViewMatInv()noexcept { return g_curMainCam->GetCamMatProjViewInv(); }
	Camera();
	~Camera();

	void FinalUpdate()override;
	void ShakeCam(const float fShakeIntensity_, const float fDuration_)noexcept;
	void PreRender()const override;

	void SetMainCam()
	{
		if (g_curMainCam)
		{
			g_curMainCam->m_bIsMainCam = false;
		}
		m_bIsMainCam = true;
		g_curMainCam = shared_from_this();
	}

	void SetObserver() noexcept { g_observer = shared_from_this(); }

	void SetNear(const float fNear_)noexcept { m_near = fNear_; }

	void SetCamFactor(const float factor_)noexcept { m_camOrthoFactor = factor_; }

	void StartChangeCamProjType()noexcept;

	PROJECTION_TYPE GetCamProjType()const { return m_eProjType; }
	void SetCamProjType(PROJECTION_TYPE _eType) { m_eProjType = _eType; }

	float GetCamFov() const { return m_fov; }
	void SetCamFov(float fov_) { m_fov = fov_; }

	const glm::mat4& GetCamMatView()const { return m_matView; }
	const glm::mat4& GetCamMatProj()const { return m_matProjection; }
	const std::pair<glm::mat4, glm::mat4> GetCamMatProjViewInv()const noexcept { return std::make_pair(m_matProjInv, m_matViewInv); }
	const std::pair<glm::mat4,glm::mat4> GetCamMatProjView()const noexcept{ return std::make_pair(m_matProjection, m_matView); }

	virtual void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath) override
	{
		writer.String("Camera");

		writer.Key("IsPerspective");
		writer.Bool(PROJECTION_TYPE::PERSPECTIVE == m_eProjType);

		writer.Key("IsMainCam");
		writer.Bool(m_bIsMainCam);
	}

	virtual void Load(string_view _dirName, const rapidjson::Value& doc, const fs::path& _loadPath) override
	{
		const rapidjson::Value& camera = doc;

		camera["IsPerspective"].GetBool() ? m_eProjType = PROJECTION_TYPE::PERSPECTIVE : m_eProjType = PROJECTION_TYPE::ORTHOGRAPHIC;

		m_bIsMainCam = camera["IsMainCam"].GetBool();

		if (m_bIsMainCam)
		{
			SetMainCam();
		}
	}
};


