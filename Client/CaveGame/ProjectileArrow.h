#pragma once

#include "ServerObject.h"

class MCTilemap;

class ProjectileArrow : public ServerObject
{
	static constexpr const int ARROW_DMG = 100;
public:
	ProjectileArrow(shared_ptr<MCTilemap> tilemap);

	void Awake() override;
	void Update() override;
private:
	
	shared_ptr<MCTilemap> m_refTilemap;
	bool m_isPinned = false;

};