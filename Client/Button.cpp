#include "pch.h"
#include "Button.h"

Button::Button(const glm::vec2 midPos, string_view strTexName, const float scaleFactor, glm::vec2 startUV, glm::vec2 endUV)
	:UI{midPos,strTexName,scaleFactor,startUV,endUV}
{
}

Button::Button()
{
}

Button::~Button()
{
}
