#include "pch.h"
#include "PointLight.h"
#include "Core.h"

PointLight::PointLight()
{
}

PointLight::~PointLight()
{
}

void PointLight::PushLightData() const noexcept
{
	UBOData& uboBlock = Mgr(Core)->GetUBOData();
	const ushort idx = uboBlock.lightCounts.x;
	PointLightData& ldata = uboBlock.pointLights[idx];
	ldata.lData.position = m_position;
	ldata.lData.ambient = m_ambient;
	ldata.lData.diffuse = m_diffuse;
	ldata.lData.specular = m_specular;
	ldata.lData.direction = m_direction;
	ldata.constant = m_fConstant;
	ldata.linear = m_fLinear;
	ldata.quadratic = m_fQuadratic;
	++uboBlock.lightCounts.x;
}

