#pragma once
#include "Resource.h"

class Shader
	:public Resource
{
	friend class ResMgr;
	friend class MeshRenderer;
private:
	GLuint m_shaderID = {};
	std::pair<string,string> loadShader(string_view _fileName);
	virtual void Load(string_view _filePath, string_view _resName)override;
public:
	Shader();
	~Shader();
	void SetUniformMat4(const glm::mat4& _mat4,string_view _uName)const;
	void Use() const;
	const GLuint GetShaderID()const noexcept { return m_shaderID; }
};

