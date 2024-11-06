#pragma once
#include "Resource.h"

class Texture
	:public Resource
{
protected:
	friend class ResMgr;
	static GLint g_curShaderID;
	static GLuint g_nextTexUnitID;
protected:
	GLuint m_texID;
	GLuint m_texUnitID;

	GLint m_texWidth;
	GLint m_texHeight;
	GLint m_texChannel;

	Texture();
	virtual void Load(string_view _filePath, string_view _resName) = 0;
public:
	virtual ~Texture();
public:
	GLuint GetTexID()const { return m_texID; }
	GLuint GetTexWidth()const { return static_cast<GLuint>(m_texWidth); }
	GLuint GetTexHeight()const { return static_cast<GLuint>(m_texHeight); }
	std::pair<GLuint, GLuint> GetTexWH()const { return std::make_pair(GetTexWidth(), GetTexHeight()); }

	virtual void BindTexture() const = 0;
	virtual void UnBindTexture()const = 0;

	virtual void BindNormalTexture()const {};
};

