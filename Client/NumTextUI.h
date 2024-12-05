#pragma once

#include "PannelUI.h"

class NumTextUI : public PannelUI
{
public:
	NumTextUI(const glm::vec2 midPos, string_view strTexName, const float scaleFactor, glm::vec2 startUV, glm::vec2 endUV);

public:
	void Render() override;
	void SetNumber(int num) { m_num = num; }

private:
	int m_num = 0;
};