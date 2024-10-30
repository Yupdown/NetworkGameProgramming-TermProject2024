#pragma once
#include "UI.h"

class Button
	:public UI
{
	friend class PannelUI;
public:
	Button();
	~Button();
	Button(const glm::vec2 midPos, string_view strTexName, const float scaleFactor = 1.f, glm::vec2 startUV = { 0,0 }, glm::vec2 endUV = { 1,1 });
};

