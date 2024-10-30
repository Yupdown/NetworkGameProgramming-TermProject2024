#pragma once
#include "pch.h"
#include "Light.h"

class PointLight
	:public Light
{
public:
	PointLight();
	~PointLight();
private:
	void PushLightData() const noexcept override;
};

