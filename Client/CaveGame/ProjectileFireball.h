#pragma once

#include "ServerObject.h"

class MCTilemap;

class ProjectileFireball : public ServerObject
{
public:
	static constexpr int PROJECTILE_DAMAGE = 100;

public:
	ProjectileFireball(shared_ptr<MCTilemap> tilemap);

	void Start() override;
	void Update() override;

private:
	shared_ptr<MCTilemap> m_refTilemap;
	shared_ptr<MeshRenderer> m_renderer;

	bool m_isPinned = false;
};

