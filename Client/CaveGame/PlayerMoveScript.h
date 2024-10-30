#pragma once
#include "MonoBehavior.h"

class PlayerMoveScript
	:public MonoBehavior
{
private:
	float m_fMoveSpeed = 1.f;
public:
	PlayerMoveScript();
	~PlayerMoveScript();

	void Update()override;
};

