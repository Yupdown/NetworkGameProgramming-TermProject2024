#include "pch.h"
#include "Texture.h"


GLint Texture::g_curShaderID;
GLuint Texture::g_nextTexUnitID = 0;

Texture::Texture()
    :Resource{RESOURCE_TYPE::NONE}
    ,m_texUnitID{g_nextTexUnitID++}
{
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texID);
}
