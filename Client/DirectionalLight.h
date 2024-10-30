#pragma once
#include "pch.h"
#include "Light.h"

class DirectionalLight
	:public Light
{
public:
	DirectionalLight();
	~DirectionalLight();
private:
	void PushLightData() const noexcept override;
};

