#include "pch.h"
#include "Texture2D.h"
#include "Camera.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "GameObj.h"
#include "Transform.h"

Texture2D::Texture2D()
{
    m_eResType = RESOURCE_TYPE::TEXTURE_2D;
}

Texture2D::~Texture2D()
{
}

void Texture2D::Load(string_view _filePath, string_view _resName)
{
    stbi_set_flip_vertically_on_load(true);

    unsigned char* texDataPointer = stbi_load(GetResPath().data(), &m_texWidth, &m_texHeight, &m_texChannel, 0);

    assert(texDataPointer);

    glGenTextures(1, &m_texID);
    glActiveTexture(GL_TEXTURE0 + m_texUnitID);
    glBindTexture(GL_TEXTURE_2D, m_texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLenum format;
    if (m_texChannel == 1)
    {
        format = GL_RED;
    }
    else if (m_texChannel == 3)
    {
        format = GL_RGB;
    }
    else if (m_texChannel == 4)
    {
        format = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_texWidth, m_texHeight, 0, format, GL_UNSIGNED_BYTE, texDataPointer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(texDataPointer);
}
