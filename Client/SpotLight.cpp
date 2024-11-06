#include "pch.h"
#include "SpotLight.h"
#include "Core.h"

SpotLight::SpotLight()
{
}

SpotLight::~SpotLight()
{
}

void SpotLight::PushLightData() const noexcept
{
	UBOData& uboBlock = Mgr(Core)->GetUBOData();
	const ushort idx = uboBlock.lightCounts.y;
	SpotLightData& ldata = uboBlock.spotLights[idx];
	ldata.lData.position = m_position;
	ldata.lData.ambient = m_ambient;
	ldata.lData.diffuse = m_diffuse;
	ldata.lData.specular = m_specular;
	ldata.lData.direction = m_direction;
	ldata.constant = m_fConstant;
	ldata.linear = m_fLinear;
	ldata.quadratic = m_fQuadratic;
	ldata.cutOff = m_cutoff;
	ldata.outerCutoff = m_outerCutoff;
	++uboBlock.lightCounts.y;
}
