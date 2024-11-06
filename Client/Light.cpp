#include "pch.h"
#include "Light.h"
#include "GameObj.h"
#include "Transform.h"
#include "Core.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"

Light::Light()
	:Component{COMPONENT_TYPE::LIGHT}
{

}

Light::~Light()
{
}

void Light::FinalUpdate()
{
	
	GetCurLight()->LightUpdate();
}

void Light::SetLightPos(const glm::vec3& _position)
{
	GetTransform()->SetLocalPosition(_position);
}

shared_ptr<Light> Light::SetCurLightType(const LIGHT_TYPE _eType)
{
	
	switch (_eType)
	{
	case LIGHT_TYPE::POINT:m_curLight = make_shared<PointLight>(); break;
	case LIGHT_TYPE::SPOT:m_curLight  = make_shared<SpotLight>(); break;
	case LIGHT_TYPE::DIRECTIONAL:m_curLight = make_shared<DirectionalLight>(); break;
	}
	*static_cast<Light*>(m_curLight.get()) = *this;
	return m_curLight;
}

void Light::PushLightData(const GLint _curShaderID,const int _idx) const
{
	const string lightLoc = std::format("lights[{}]", _idx);
	glUniform3fv(glGetUniformLocation(_curShaderID, (lightLoc + ".position").data()), 1, glm::value_ptr(GetLightPos()));
	glUniform3fv(glGetUniformLocation(_curShaderID, (lightLoc + ".ambient").data()), 1, glm::value_ptr(GetLightAmbient()));
	glUniform3fv(glGetUniformLocation(_curShaderID, (lightLoc + ".diffuse").data()), 1, glm::value_ptr(GetLightDiffuse()));
	glUniform3fv(glGetUniformLocation(_curShaderID, (lightLoc + ".specular").data()), 1, glm::value_ptr(GetLightSpecular()));
}

void Light::PushLightData() const noexcept
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

void Light::LightUpdate() noexcept
{
	const auto lightTrans = GetTransform();
	
	//lightTrans->GetWaitLock().Wait();
	m_position = lightTrans->GetWorldPosition();
	m_direction = glm::normalize(lightTrans->GetLook());
	if (m_curLight)
	{
		m_curLight->m_position = m_position;
		m_curLight->m_direction = m_direction;
	}
	if (IsZeroVector(m_position))
	{
		m_position = glm::vec3{ 0.f,1.f,0.f };
	}
}
