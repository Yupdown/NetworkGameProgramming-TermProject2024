#pragma once
#include "Texture.h"

class Texture2D
	:public Texture
{
private:

public:
	Texture2D();
	~Texture2D();

	virtual void Load(string_view _filePath, string_view _resName)override;

	virtual void BindTexture() const override
	{
		glActiveTexture(GL_TEXTURE0 + m_texUnitID);
		glGetIntegerv(GL_CURRENT_PROGRAM, &Texture::g_curShaderID);
		glBindTexture(GL_TEXTURE_2D, m_texID);
		glUniform1i(glGetUniformLocation(Texture::g_curShaderID, "uTexture2D"), m_texUnitID);
	}

	virtual void UnBindTexture()const override
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	virtual void BindNormalTexture()const override
	{
		glGetIntegerv(GL_CURRENT_PROGRAM, &Texture::g_curShaderID);
		glBindTexture(GL_TEXTURE_2D, m_texID);
		glUniform1i(glGetUniformLocation(Texture::g_curShaderID, "uNormalTexture"), m_texUnitID);
	}
};

