#pragma once
#include "Component.h"

class ProjArrow
	:public Component
{
	static constexpr float G_ARROW_REMAIN_TIME = 3.f;
	static constexpr int G_ARROW_DMG = 1;
public:
	void Update(const float DT)override;
private:
	float m_accArrowTime = 0.f;
	bool m_isPinned = false;
};

