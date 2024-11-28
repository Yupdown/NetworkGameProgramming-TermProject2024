#pragma once
#include "ServerObject.h"

class MCTilemap;

class Player
	:public ServerObject
{
	std::unordered_map<string, shared_ptr<GameObj>> g_mapParticleUniqueObject;
protected:
	shared_ptr<MCTilemap> m_refTilemap;

	shared_ptr<GameObj> m_rendererObj;
	shared_ptr<GameObj> m_particlePrefab;

	float m_fMoveSpeed = 100.f;
	float m_fMoveTime = 0.0f;

	glm::vec3 m_vVelocity = glm::zero<glm::vec3>();
	glm::vec3 m_vAccelation = glm::zero<glm::vec3>();
	float m_vVelocityMax = 7.5f;
	bool m_bGround = false;

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
	int m_rendererTextureId = 0;

protected:
	shared_ptr<Transform> m_pCacheMyTransform;
	bool m_bIsHero = false;
	bool m_bForceSendData = false;

protected:
	void HandleCollision()noexcept;

public:
	void UpdateBodyRotation(float yaw);
	void UpdateHeadRotation(float pitch, float yaw);

	void SetRendererTexture(int id);
	void ValidateRendererTexture();
	void DestroyBlock(const glm::ivec3& hitTilePosition)noexcept;
	void CreateBlock(const glm::ivec3& hitTilePosition, uint8_t tile_id) noexcept;
	bool IsHero()const noexcept { return m_bIsHero; }

public:
	Player(shared_ptr<MCTilemap> tilemap);
	~Player();

	void Start()override;
	void Update()override;
	void OnObjectDamaged(int value)override;

	class ProjectileArrow* Fire(const glm::vec3& arrow_pos, const float x_,const float y_)noexcept;

public:
	virtual void InitializeRenderer();
	virtual void UpdateRenderer();
};

