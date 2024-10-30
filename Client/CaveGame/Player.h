#pragma once
#include "GameObj.h"

class MCTilemap;

class Player
	:public GameObj
{
	std::unordered_map<string, shared_ptr<GameObj>> g_mapParticleUniqueObject;
private:
	shared_ptr<MCTilemap> m_refTilemap;

	shared_ptr<GameObj> m_rendererObj;
	shared_ptr<GameObj> m_cameraAnchor;
	shared_ptr<GameObj> m_cameraObj;
	shared_ptr<GameObj> m_cursorBlockObj;
	shared_ptr<GameObj> m_particlePrefab;

	std::function<void(void)> m_fpChangeCamMode[3];

	float m_fMoveSpeed = 10.f;
	float m_fMoveTime = 0.0f;
	float m_fCamSensivity = 0.1f;

	glm::vec3 m_vVelocity = glm::zero<glm::vec3>();
	glm::vec3 m_vAccelation = glm::zero<glm::vec3>();
	float m_vVelocityMax = 0.75f;
	bool m_bGround = false;

	glm::vec3 m_cameraAngleAxis = glm::zero<glm::vec3>();
	glm::vec3 m_cameraAngleAxisSmooth = glm::zero<glm::vec3>();

	shared_ptr<Transform> m_transformHead;
	shared_ptr<Transform> m_transformHeadOut;
	shared_ptr<Transform> m_transformLArm;
	shared_ptr<Transform> m_transformLArmOut;
	shared_ptr<Transform> m_transformRArm;
	shared_ptr<Transform> m_transformRArmOut;
	shared_ptr<Transform> m_transformLLeg;
	shared_ptr<Transform> m_transformLLegOut;
	shared_ptr<Transform> m_transformRLeg;
	shared_ptr<Transform> m_transformRLegOut;

	float m_rendererBodyAngleY = 0.0f;

	shared_ptr<Camera> m_pCamera;

	ushort m_curCamMode = 0;
private:
	shared_ptr<GameObj> CreateCursorBlockObj() const;
	void ChangeCamType()noexcept;
	void UpdatePlayerCamFpsMode()noexcept;
	void MoveByView(const glm::vec3& vDelta);
	void UpdateRenderer();
	void UpdateCameraTransform(shared_ptr<Transform> pCameraTransfrom);
public:
	Player(shared_ptr<MCTilemap> tilemap);
	~Player();

	void Start()override;

	void Update()override;
	
	glm::vec3 GetPlayerLook() const noexcept;

	void Fire()noexcept;

	void InitCamDirection()noexcept;
	void UpdateTileManipulation()noexcept;
};

