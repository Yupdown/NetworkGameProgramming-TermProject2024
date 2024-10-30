#pragma once

class Shader;

class ShaderMgr
	:public Singleton<ShaderMgr>
{
	friend class Singleton;
	ShaderMgr();
	~ShaderMgr();
private:
	unordered_map<string, shared_ptr<Shader>> m_mapShader;
public:
	void Init();
	void UseShader(string_view _shaderName)const;
	shared_ptr<Shader> GetShader(string_view _shaderName)const;
};

