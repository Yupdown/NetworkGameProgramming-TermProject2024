#pragma once

#include "pch.h"
#include <GameObj.h>

class EnderDragonRenderer : public GameObj
{
public:
	EnderDragonRenderer();

	void Start() override;

private:
	shared_ptr<GameObj> m_rendererObject;
};