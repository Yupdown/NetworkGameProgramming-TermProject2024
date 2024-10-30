#include "pch.h"
#include "DirectionalLight.h"
#include "Core.h"

DirectionalLight::DirectionalLight()
{
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::PushLightData() const noexcept
{
	UBOData& uboBlock = Mgr(Core)->GetUBOData();
	DirectionalLightData& ldata = uboBlock.dirLight;
	ldata.lData.position = m_position;
	ldata.lData.ambient = m_ambient;
	ldata.lData.diffuse = m_diffuse;
	ldata.lData.specular = m_specular;
	ldata.lData.direction = m_direction;
	++uboBlock.lightCounts.z;
}
