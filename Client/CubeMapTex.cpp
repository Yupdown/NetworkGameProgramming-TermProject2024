#include "pch.h"
#include "CubeMapTex.h"


CubeMapTex::CubeMapTex()
{
	m_eResType = RESOURCE_TYPE::TEXTURE_CUBE;
}

CubeMapTex::~CubeMapTex()
{
}

void CubeMapTex::Load(string_view _filePath, string_view _resName)
{
	stbi_set_flip_vertically_on_load(true);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenTextures(1, &m_texID);
	glActiveTexture(GL_TEXTURE0 + m_texUnitID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);

	bool bInitFlag = false;

	fs::directory_iterator cubTexIter{GetResPath()};

	int texCnt = 0;
	GLenum format;
	string tempPath{};

	for (const auto& cubTex : cubTexIter)
	{
		tempPath = cubTex.path().string();

		unsigned char* texDataPointer = stbi_load(tempPath.data(), &m_texWidth, &m_texHeight, &m_texChannel, 0);

		assert(texDataPointer);

		if (!bInitFlag)
		{
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

			glTextureStorage2D(m_texID, 1, GL_RGBA8, m_texWidth, m_texHeight);
			bInitFlag = true;
		}

		glTextureSubImage3D(m_texID, 0, 0, 0, texCnt++, m_texWidth, m_texHeight, 1,
			format, GL_UNSIGNED_BYTE, texDataPointer);

		stbi_image_free(texDataPointer);
	}

	if (1 == texCnt)
	{
		unsigned char* texDataPointer = stbi_load(tempPath.data(), &m_texWidth, &m_texHeight, &m_texChannel, 0);

		assert(texDataPointer);
		
		for (int i = 0; i < 5; ++i)
		{
			glTextureSubImage3D(m_texID, 0, 0, 0, texCnt++, m_texWidth, m_texHeight, 1,
				format, GL_UNSIGNED_BYTE, texDataPointer);
		}

		stbi_image_free(texDataPointer);
	}

	glGenerateTextureMipmap(m_texID);
	glTextureParameterf(m_texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameterf(m_texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
