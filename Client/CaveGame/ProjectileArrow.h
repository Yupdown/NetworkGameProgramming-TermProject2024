#pragma once

#include "ServerObject.h"

class MCTilemap;

class ProjectileArrow : public ServerObject
{
public:
	ProjectileArrow(shared_ptr<MCTilemap> tilemap);

	void Awake() override;
	void Update() override;

private:
	shared_ptr<MCTilemap> m_refTilemap;
	bool m_isPinned = false;
};