#pragma once
#include "GameObj.h"

class MCTilemap;

class PlayerShadow:
	public GameObj
{
private:
	const shared_ptr<const MCTilemap> m_pTilemap;
	float m_fPlayerOriginY = 0.f;
	float m_fShadowAlpha = 1.f;
public:
	PlayerShadow(const shared_ptr<const MCTilemap>& tilemap);
	~PlayerShadow();

	void Start()override;
	void Update()override;
	void Render()override;
};