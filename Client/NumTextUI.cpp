#include "pch.h"
#include "NumTextUI.h"

NumTextUI::NumTextUI(const glm::vec2 midPos, const float scaleFactor) : PannelUI(midPos, "ascii.png", 0.0f)
{
	for (int i = 0; i < 10; ++i)
	{
		glm::vec2 unit = glm::one<glm::vec2>() / 16;
		glm::vec2 startUV = glm::vec2(unit.x * i, unit.y * 3);
		glm::vec2 endUV = startUV + unit;
		m_digit1[i] = make_shared<PannelUI>(midPos, "ascii.png", scaleFactor, startUV, endUV);
		m_digit10[i] = make_shared<PannelUI>(midPos - glm::vec2(2.0f, 0.0f), "ascii.png", scaleFactor, startUV, endUV);
	}
}

void NumTextUI::SetNumber(int num)
{
	int digit1 = num % 10;
	int digit10 = num / 10;
	for (int i = 0; i < 10; ++i)
	{
		m_digit1[i]->SetActivate(digit1 == i);
		m_digit10[i]->SetActivate(digit10 > 0 && digit10 == i);
	}
}

void NumTextUI::Start()
{
	for (int i = 0; i < 10; ++i)
	{
		AddChild(m_digit1[i]);
		AddChild(m_digit10[i]);
	}

	PannelUI::Start();
}
