#include "pch.h"
#include "PathMgr.h"

PathMgr::PathMgr()
{
}

PathMgr::~PathMgr()
{
}

void PathMgr::Init()
{
	m_curPath = fs::current_path();
	m_resPath = m_curPath.parent_path() / "Resource";

	m_shaderPath = m_resPath / "Shader";
	m_objPath = m_resPath / "Model";
	m_soundPath = m_resPath / "Sound";
	m_texPath = m_resPath / "Texture";
	m_savePath = m_resPath / "Save";
}
