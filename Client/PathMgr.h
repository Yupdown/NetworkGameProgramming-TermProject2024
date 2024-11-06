#pragma once

class PathMgr
	:public Singleton<PathMgr>
{
	friend class Singleton;
	PathMgr();
	~PathMgr();
private:
	fs::path m_curPath;
	fs::path m_resPath;

	fs::path m_shaderPath;
	fs::path m_objPath;
	fs::path m_soundPath;
	fs::path m_texPath;
	fs::path m_savePath;
public:
	void Init();
	auto GetResourcePath()const { return m_resPath; }

	auto GetShaderPath()const { return m_shaderPath; }
	auto GetObjPath()const { return m_objPath; }
	auto GetSoundPath()const { return m_soundPath; }
	auto GetTexPath()const { return m_texPath; }
	auto GetSavePath()const { return m_savePath; }
};

