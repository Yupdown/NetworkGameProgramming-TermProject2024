#pragma once
#include "PannelUI.h"

class Button;

class PannelProgressUI : public PannelUI
{
public:
	PannelProgressUI(const glm::vec2 midPos, string_view strTexName, const float scaleFactor = 1.f, glm::vec2 startUV = { 0,0 }, glm::vec2 endUV = { 1,1 });
	~PannelProgressUI();
	void Render() override;
	void SetProgress(float _fProgress) { m_fProgress = _fProgress; }

private:
	float m_fProgress = 1.0f;
};