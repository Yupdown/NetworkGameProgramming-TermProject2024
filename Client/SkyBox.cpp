#include "pch.h"
#include "SkyBox.h"
#include "AssimpMgr.h"
#include "ResMgr.h"
#include "Camera.h"
#include "Texture2D.h"
#include "CubeMapTex.h"

SkyBox::SkyBox(SKYBOX_TYPE _eSkyBoxType,string_view _strSkyBoxName, string_view _strTextureName)
{
	m_eResType = RESOURCE_TYPE::SKYBOX;
	if (SKYBOX_TYPE::SPHERE == _eSkyBoxType)
	{
		*static_cast<GameObj*>(this) = *Mgr(AssimpMgr)->Load("SkyBoxShaderSphere.glsl", _strSkyBoxName);
		m_pSkyBoxShader = Mgr(ResMgr)->GetRes<Shader>("SkyBoxShaderSphere.glsl");
		m_pSkyBoxTex = Mgr(ResMgr)->GetRes<Texture2D>(_strTextureName);
	}
	else
	{
		*static_cast<GameObj*>(this) = *Mgr(AssimpMgr)->Load("SkyBoxShaderCube.glsl", _strSkyBoxName);
		m_pSkyBoxShader = Mgr(ResMgr)->GetRes<Shader>("SkyBoxShaderCube.glsl");
		m_pSkyBoxTex = Mgr(ResMgr)->GetRes<CubeMapTex>(_strTextureName);
	}
}

SkyBox::~SkyBox()
{
}

void SkyBox::Render() noexcept
{
	glDepthFunc(GL_LEQUAL);
	m_pSkyBoxShader->Use();
	m_pSkyBoxShader->SetUniformMat4(glm::mat4(glm::mat3(Camera::GetMainCamViewMat())), "uSkyBoxViewMat");
	m_pSkyBoxTex->BindTexture();
	GameObj::Render();
	m_pSkyBoxTex->UnBindTexture();
	glDepthFunc(GL_LESS);
}
