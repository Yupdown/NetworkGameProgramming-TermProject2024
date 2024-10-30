#pragma once

enum class RESOURCE_TYPE 
{
	NONE,
	GAMEOBJECT, 
	COMPONENT,
	MATERIAL,
	MESH,
	SHADER,
	TEXTURE_2D,
	TEXTURE_CUBE,
	MODEL,
	SKYBOX,

	END
};

enum
{
	RESOURCE_TYPE_COUNT = static_cast<GLuint>(RESOURCE_TYPE::END)
};

class Resource
{
	friend class ResMgr;
private:
	string m_strResName;
	string m_strResPath;
protected:
	Resource(RESOURCE_TYPE _eType);

	RESOURCE_TYPE m_eResType = RESOURCE_TYPE::NONE;


	void SetResName(string_view _strFileName) { m_strResName = _strFileName; }
	void SetResPath(string_view _strFilePath) { m_strResPath = _strFilePath; }

	virtual void Load(string_view _filePath, string_view _resName){}
	virtual void Load(string_view _dirName,const fs::path& _loadPath = {}) {}
	virtual void Load(string_view _dirName, const rapidjson::Value& doc,const fs::path& _loadPath){}

	virtual void Save(string_view _resName,const fs::path& _savePath = {}) {}
	virtual void Save(string_view _resName,rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer,const fs::path& _savePath){}
public:
	virtual ~Resource();
	RESOURCE_TYPE GetResType()const { return m_eResType; }
	const string& GetResName()const { return m_strResName; }
	const string& GetResPath()const { return m_strResPath; }
};

