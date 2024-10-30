#pragma once
#include "pch.h"
#include "Light.h"

class SpotLight
	:public Light
{
private:
    float m_cutoff = glm::radians(30.f);
    float m_outerCutoff = glm::radians(22.5f);
public:
    SpotLight();
    ~SpotLight();
public:
    void SetCutOff(const float fCutOff_)noexcept { m_cutoff = fCutOff_; }
    void SetOuterCutOff(const float fOuterCutOff_)noexcept { m_outerCutoff = fOuterCutOff_; }
private:
    void PushLightData() const noexcept override;
};

