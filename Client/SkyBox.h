#pragma once
#include "GameObj.h"

class Shader;
class Texture;

class SkyBox
	:public GameObj
{
private:
	shared_ptr<Shader> m_pSkyBoxShader;
	shared_ptr<Texture> m_pSkyBoxTex;
public:
	SkyBox(SKYBOX_TYPE _eSkyBoxType,string_view _strSkyBoxName, string_view _strTextureName);
	~SkyBox();
public:
	void Render()noexcept;
};

