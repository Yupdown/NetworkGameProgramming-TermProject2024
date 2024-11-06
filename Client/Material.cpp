#include "pch.h"
#include "Material.h"
#include "Texture.h"
#include "ResMgr.h"

Material::Material()
	:Resource{RESOURCE_TYPE::MATERIAL}
{
}

Material::~Material()
{
}

void Material::PushMaterialData()
{
	static GLint curShaderID;
	glGetIntegerv(GL_CURRENT_PROGRAM, &curShaderID);
	static const string materialLoc = "material";

	glUniform3fv(glGetUniformLocation(curShaderID, (materialLoc + ".ambient").data()), 1, glm::value_ptr(m_ambient));
	glUniform3fv(glGetUniformLocation(curShaderID, (materialLoc + ".diffuse").data()), 1, glm::value_ptr(m_diffuse));
	glUniform3fv(glGetUniformLocation(curShaderID, (materialLoc + ".specular").data()), 1, glm::value_ptr(m_specular));
	glUniform1f(glGetUniformLocation(curShaderID, (materialLoc + ".shininess").data()),m_shininess);
}

bool Material::AddTexture2D(string_view _strTexName)
{
	auto pTex = Mgr(ResMgr)->GetRes<Texture2D>(_strTexName);
	if (pTex)
	{
		AddTexture(std::move(pTex));
		return true;
	}
	else
	{
		return false;
	}
}

void Material::AddTextureCube(string_view _strTexName)
{
	AddTexture(Mgr(ResMgr)->GetRes<CubeMapTex>(_strTexName));
}


