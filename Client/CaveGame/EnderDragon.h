#pragma once
#include "ServerObject.h"

class EnderDragon
	: public ServerObject
{
public:
	EnderDragon();

	void Start() override;
	void Update()override;
	void OnObjectDamaged(int value) override;
private:
	float m_rendererBodyAngleY = 0.f;
	shared_ptr<class EnderDragonRenderer> m_renderDragonRenderer;
};

