#pragma once
#include "Texture.h"


class CubeMapTex
	:public Texture
{
private:
	
public:
	CubeMapTex();
	~CubeMapTex();

	virtual void Load(string_view _filePath, string_view _resName)override;

	virtual void BindTexture() const override
	{
		glActiveTexture(GL_TEXTURE0 + m_texUnitID);
		glGetIntegerv(GL_CURRENT_PROGRAM, &Texture:: g_curShaderID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
		glUniform1i(glGetUniformLocation(Texture::g_curShaderID, "uCubeTexture"), m_texUnitID);
	}

	virtual void UnBindTexture()const override
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
};

