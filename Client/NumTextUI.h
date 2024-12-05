#pragma once

#include "PannelUI.h"

class NumTextUI : public PannelUI
{
public:
	NumTextUI(const glm::vec2 posLB, const float scaleFactor = 1.f);

public:
	void Start() override;
	void SetNumber(int num);

private:
	std::shared_ptr<PannelUI> m_digit1[10];
	std::shared_ptr<PannelUI> m_digit10[10];
};