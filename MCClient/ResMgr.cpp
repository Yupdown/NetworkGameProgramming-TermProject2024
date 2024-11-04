#include "pch.h"
#include "ResMgr.h"
#include "PathMgr.h"

ResMgr::ResMgr()
{
	m_mapResNameAndMeshData.reserve(1024);
}

ResMgr::~ResMgr()
{
}

void ResMgr::InitShader()
{
	fs::directory_iterator ShaderIter{ Mgr(PathMgr)->GetShaderPath() };

	for (const auto& shader : ShaderIter)
	{
		Load<Shader>(shader.path().string(), shader.path().filename().string());
	}
}

void ResMgr::InitTexture2D()
{
	fs::recursive_directory_iterator Texture2DIter{ Mgr(PathMgr)->GetTexPath() / "Texture2D" };

	for (const auto& texIter : Texture2DIter)
	{
		if (texIter.is_regular_file())
		{
			Load<Texture2D>(texIter.path().string(), texIter.path().filename().string());
		}
	}
}

void ResMgr::InitTextureCube()
{
	fs::directory_iterator TextureCubeIter{ Mgr(PathMgr)->GetTexPath() / "TextureCube" };

	for (const auto& texIter : TextureCubeIter)
	{
		Load<CubeMapTex>(texIter.path().string(), texIter.path().filename().string());
	}
}

void ResMgr::Init()
{
	InitShader();
	InitTexture2D();
	InitTextureCube();
}
