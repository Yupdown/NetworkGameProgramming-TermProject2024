#pragma once

#include "pch.h"
#include <GameObj.h>

class DeathObject : public GameObj
{
public:
	DeathObject(shared_ptr<GameObj> renderObj);
	void Start() override;
	void Update() override;

private:
	float m_fLifeTime = 0.0f;
	shared_ptr<GameObj> m_renderObj;
};