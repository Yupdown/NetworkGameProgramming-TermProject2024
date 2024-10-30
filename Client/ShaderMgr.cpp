#include "pch.h"
#include "ShaderMgr.h"
#include "Shader.h"
#include "PathMgr.h"

ShaderMgr::ShaderMgr()
{
}

ShaderMgr::~ShaderMgr()
{
}

void ShaderMgr::Init()
{
	fs::directory_iterator ShaderIter{ Mgr(PathMgr)->GetShaderPath() };

	for (const auto& shader : ShaderIter)
	{
		auto pShader = new Shader{ (shader.path() / shader.path().filename()).string() + ".vert",(shader.path() / shader.path().filename()).string() + ".frag"};
		m_mapShader.emplace(shader.path().filename().string(), pShader);
	}
}

void ShaderMgr::UseShader(string_view _shaderName) const
{
	auto iter = m_mapShader.find(_shaderName.data());
	if (m_mapShader.end() == iter)
	{
		assert(false);
	}
	iter->second->Use();
}

shared_ptr<Shader> ShaderMgr::GetShader(string_view _shaderName) const
{
	auto iter = m_mapShader.find(_shaderName.data());
	if (m_mapShader.end() == iter)
	{
		assert(false);
	}
	return iter->second;
}
