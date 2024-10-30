#pragma once

class GameObj;
class Mesh;
class Material;
class Animator;
struct asAnimationNode;
struct Animation;
class MyPolygon;
struct SimpleVertex;
class Model;

class AssimpMgr
	:public Singleton<AssimpMgr>
{
	friend class Singleton;
	AssimpMgr();
	~AssimpMgr();
private:
	Assimp::Importer m_importer = {};
	vector<std::future<void>> m_wait;
	vector<std::future<void>> m_fpSetBuf;
	//SpinLock m_spinLock = {};
private:
	shared_ptr<Model> ProcessNode(aiNode* pNode, const aiScene* pAiScene,const glm::mat4 parentWorld=glm::mat4{1.f})noexcept;
	shared_ptr<Mesh> ProcessMesh(aiMesh* pAiMesh)noexcept;
	//void ProcessBone(vector<Vertex>& vertices, aiNode* pNode, const aiScene* pAiScene);
	shared_ptr<Material> ProcessMaterial(aiMesh* pAiMesh,const aiScene* pAiScene)noexcept;
private:
	shared_ptr<Animation> ProcessAnimation(aiAnimation* pAiAnim,const aiScene* pAiScene);
	shared_ptr<asAnimationNode> ParseAnimNode(shared_ptr<Animation> pAnim, aiNodeAnim* pAnimNode);
	void ReadKeyFrameData(shared_ptr<Animation> pAnim, aiNode* pNode, map<string, shared_ptr<asAnimationNode>>& cache);
private:
	shared_ptr<MyPolygon> ProcessNodePoly(aiNode* pNode, const aiScene* pAiScene);
	std::pair<vector<SimpleVertex>, vector<GLuint>> ProcessMeshPoly(aiMesh* pAiMesh);
private:
	shared_ptr<GameObj> ProcessNodeGameObj(string_view strShaderName_, aiNode* pNode, const aiScene* pAiScene)noexcept;
public:
	void Init();
	shared_ptr<GameObj> Load(string_view _strShaderName ,string_view _strModelFileName)noexcept;
	shared_ptr<Model> LoadModel(string_view _strModelFileName)noexcept;
	shared_ptr<MyPolygon> LoadPoly(string_view _strPolyFileName);
	shared_ptr<GameObj> LoadAllPartsAsGameObj(string_view _strShaderName, string_view _strModelFileName)noexcept;
};

