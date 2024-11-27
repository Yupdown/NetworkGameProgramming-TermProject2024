#pragma once

#include "ServerObject.h"

class MCTilemap;

class ProjectileArrow : public ServerObject
{
public:
	ProjectileArrow(shared_ptr<MCTilemap> tilemap);

	void Awake() override;
	void Update() override;
	const auto GetArrowLocalID()const noexcept { return m_arrow_local_id; }
private:
	const uint32_t m_arrow_local_id = g_arrow_local_id++;
	shared_ptr<MCTilemap> m_refTilemap;
	bool m_isPinned = false;


	static inline uint32_t g_arrow_local_id = 0;
};