#include "pch.h"
#include "AssimpMgr.h"
#include "GameObj.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "PathMgr.h"
#include "Texture2D.h"
#include "ThreadMgr.h"
#include "Animator.h"
#include "MyPolygon.h"
#include "Model.h"
#include "ResMgr.h"

static unordered_map<string, int> g_tempBoneMap;
static vector<BoneInfo> g_tempBoneVec;
static int boneCounter = 0;
static string tempName;

void UpdateBoneParentIndices(aiNode* pNode);
glm::mat4 GetBoneWorldTransform(const int idx);

static glm::mat4 ConvertAiMatrixToGLM(const aiMatrix4x4& from)
{
    glm::mat4 to;

    to[0][0] = from.a1; to[1][0] = from.b1; to[2][0] = from.c1; to[3][0] = from.d1;
    to[0][1] = from.a2; to[1][1] = from.b2; to[2][1] = from.c2; to[3][1] = from.d2;
    to[0][2] = from.a3; to[1][2] = from.b3; to[2][2] = from.c3; to[3][2] = from.d3;
    to[0][3] = from.a4; to[1][3] = from.b4; to[2][3] = from.c4; to[3][3] = from.d4;

    return glm::transpose(to);
}

static void SetTransformation(const shared_ptr<Transform>& pTrans, aiNode* pNode)
{
    const glm::mat4 transformMat = ConvertAiMatrixToGLM(pNode->mTransformation);

    const glm::vec3 translation = glm::vec3(transformMat[3]);

    pTrans->SetLocalPosition(translation);

    glm::vec3 scale;
    scale.x = glm::length(glm::vec3(transformMat[0]));
    scale.y = glm::length(glm::vec3(transformMat[1]));
    scale.z = glm::length(glm::vec3(transformMat[2]));

    pTrans->SetLocalScale(scale);

    glm::mat4 rotationMatrix = transformMat;
    rotationMatrix[0] = glm::normalize(rotationMatrix[0]);
    rotationMatrix[1] = glm::normalize(rotationMatrix[1]);
    rotationMatrix[2] = glm::normalize(rotationMatrix[2]);

    pTrans->SetLocalRotation(glm::quat_cast(rotationMatrix));
}

AssimpMgr::AssimpMgr()
{
}

AssimpMgr::~AssimpMgr()
{
}

shared_ptr<Model> AssimpMgr::ProcessNode(aiNode* pNode, const aiScene* pAiScene, const glm::mat4 parentWorld)noexcept
{
    auto pModel = make_shared<Model>();
   // auto pMeshRenderer = pObj->AddComponent<MeshRenderer>();
   // pMeshRenderer->SetShader(_strShaderName);
   // pObj->SetObjName(pNode->mName.C_Str());
    pModel->SetResName(pNode->mName.C_Str());

    auto res_future = make_shared<vector<std::future<shared_ptr<Mesh>>>>();
    res_future->reserve(pNode->mNumMeshes);

    for (GLuint i = 0; i < pNode->mNumMeshes; ++i)
    {
        auto pAiMesh = pAiScene->mMeshes[pNode->mMeshes[i]];

        res_future->emplace_back(Mgr(ThreadMgr)->EnqueueTaskFuture(&AssimpMgr::ProcessMesh, this, pAiMesh));
        
        auto pMaterial = ProcessMaterial(pAiMesh, pAiScene);

        pModel->AddMaterial(std::move(pMaterial));
    }
    
    pModel->m_matLocal = ConvertAiMatrixToGLM(pNode->mTransformation);
    glm::mat4 cacheMat = glm::mat4{ 1.f };
    if (pNode->mParent)
    {
        cacheMat = pModel->m_matWorld = parentWorld * pModel->m_matLocal;
    }
    else
    {
        cacheMat = pModel->m_matWorld = pModel->m_matLocal;
    }
    pModel->SetModelMat(pModel->m_matLocal);

    m_fpSetBuf.emplace_back(std::async(std::launch::deferred,[cacheMat,this,pNode,pAiScene,pModel, res_future = std::move(res_future)]()mutable noexcept {
        for (auto& f : *res_future)
        {
            auto pMesh = f.get();
            ///*if (pAiScene->HasAnimations())
            //{
            //    ProcessBone(pMesh->GetVertices(), pNode, pAiScene);
            //}*/
            pMesh->SetBuffers();
            Mgr(ResMgr)->AddMeshList(tempName, pModel->GetResName(), cacheMat, pMesh->m_vecVertex, pMesh->m_vecIdx);
            pModel->AddMesh(std::move(pMesh));
        }}));

    for (GLuint i = 0; i < pNode->mNumChildren; ++i)
    {
        pModel->AddChild(ProcessNode(pNode->mChildren[i], pAiScene,pModel->m_matWorld));
    }

    return pModel;
}

shared_ptr<Mesh> AssimpMgr::ProcessMesh(aiMesh* pAiMesh)noexcept
{
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vertices.reserve(pAiMesh->mNumVertices);
    indices.reserve(pAiMesh->mNumFaces * 3);
   
    auto f  = Mgr(ThreadMgr)->EnqueueTaskFuture([&]() {
        for (GLuint i = 0; i < pAiMesh->mNumVertices; ++i)
        {
            Vertex v;

            v.position.x = pAiMesh->mVertices[i].x;
            v.position.y = pAiMesh->mVertices[i].y;
            v.position.z = pAiMesh->mVertices[i].z;

            if (pAiMesh->HasNormals())
            {
                v.normal.x = pAiMesh->mNormals[i].x;
                v.normal.y = pAiMesh->mNormals[i].y;
                v.normal.z = pAiMesh->mNormals[i].z;
            }

            if (pAiMesh->HasTangentsAndBitangents())
            {
                v.tangent.x = pAiMesh->mTangents[i].x;
                v.tangent.y = pAiMesh->mTangents[i].y;
                v.tangent.z = pAiMesh->mTangents[i].z;
            }

            if (pAiMesh->mTextureCoords[0])
            {
                v.uv.x = pAiMesh->mTextureCoords[0][i].x;
                v.uv.y = pAiMesh->mTextureCoords[0][i].y;
            }

            if (pAiMesh->mColors[0])
            {
                v.color.r = pAiMesh->mColors[0][i].r;
                v.color.g = pAiMesh->mColors[0][i].g;
                v.color.b = pAiMesh->mColors[0][i].b;
                v.color.a = pAiMesh->mColors[0][i].a;
            }

            vertices.emplace_back(v);
        }
        });

	for (GLuint i = 0; i < pAiMesh->mNumFaces; ++i)
	{
		auto aiFace = pAiMesh->mFaces[i];
		for (GLuint j = 0; j < aiFace.mNumIndices; ++j)
		{
			indices.emplace_back(aiFace.mIndices[j]);
		}
	}
    f.get();

    return make_shared<Mesh>(std::move(vertices), std::move(indices));
}

//void AssimpMgr::ProcessBone(vector<Vertex>& vertices, aiNode* pNode, const aiScene* pAiScene)
//{
//    for (GLuint m = 0; m < pNode->mNumMeshes; ++m)
//    {
//        auto pAiMesh = pAiScene->mMeshes[pNode->mMeshes[m]];
//
//        if (pAiMesh->HasBones())
//        {
//            for (GLuint i = 0; i < pAiMesh->mNumBones; ++i)
//            {
//                auto bone = pAiMesh->mBones[i];
//
//				if (g_tempBoneMap.find(bone->mName.C_Str()) == g_tempBoneMap.end())
//				{
//					BoneInfo newBone;
//					newBone.name = bone->mName.C_Str();
//					newBone.offsetMat = glm::transpose(glm::make_mat4(&bone->mOffsetMatrix.a1));
//					g_tempBoneMap[newBone.name] = (int)g_tempBoneVec.size();
//					newBone.index = (int)g_tempBoneMap.size();
//					if (pNode->mParent)
//					{
//                        auto iter = g_tempBoneMap.find(pNode->mParent->mName.C_Str());
//                        if(g_tempBoneMap.end() !=iter && iter->second >=0)
//						    newBone.parentIndex = g_tempBoneMap[pNode->mParent->mName.C_Str()];
//					}
//					g_tempBoneVec.emplace_back(newBone);
//				}
//                
//                for (GLuint j = 0; j < bone->mNumWeights; ++j)
//                {
//                    auto weight = bone->mWeights[j];
//                    auto& v = vertices[weight.mVertexId];
//                    for (int slot = 0; slot < 4; ++slot)
//                    {
//                        if (v.boneWeights[slot] == 0.0f)
//                        {
//                            v.boneIds[slot] = i;
//                            v.boneWeights[slot] = weight.mWeight;
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

shared_ptr<Material> AssimpMgr::ProcessMaterial(aiMesh* pAiMesh,const aiScene* pAiScene)noexcept
{
    auto pAiMaterial = pAiScene->mMaterials[pAiMesh->mMaterialIndex];

    auto pMaterial = make_shared<Material>();

    aiColor3D color(0.f, 0.f, 0.f);

    // Ambient color
    m_wait.emplace_back(Mgr(ThreadMgr)->EnqueueTaskFuture([pMaterial, color, pAiMaterial]()mutable {
        if (AI_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color)) {
            pMaterial->SetMaterialAmbient(glm::vec3(color.r, color.g, color.b));
        }

        // Diffuse color
        if (AI_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color)) {
            pMaterial->SetMaterialDiffuse(glm::vec3(color.r, color.g, color.b));
        }

        // Specular color
        if (AI_SUCCESS == pAiMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color)) {
            pMaterial->SetMaterialSpecular(glm::vec3(color.r, color.g, color.b));
        }

        // Shininess
        float shininess = 0.0f;
        if (AI_SUCCESS == pAiMaterial->Get(AI_MATKEY_SHININESS, shininess)) {
            pMaterial->SetMaterialShine(shininess);
        }
        }));
    for (int i = 0; i < (int)(aiTextureType::aiTextureType_TRANSMISSION)+1; ++i)
    {
        aiString path;
        if (pAiMaterial->GetTexture((aiTextureType)(i), 0, &path) == AI_SUCCESS)
        {
            std::cout << path.C_Str() << std::endl;
            if (!pMaterial->AddTexture2D(path.C_Str()))
            {
                const string temp = path.C_Str();
                std::cout << temp << std::endl;
                if (!pMaterial->AddTexture2D(temp.substr(temp.rfind('\\') + 1)))
                {
                    pMaterial->AddTexture2D(temp.substr(temp.rfind("/") + 1));
                }
                std::cout << temp.substr(temp.rfind('\\') + 1) << std::endl;
                std::cout << temp.substr(temp.rfind("/") + 1) << std::endl;
                break;
            }
        }
    }
   
    return pMaterial;
}

shared_ptr<Animation> AssimpMgr::ProcessAnimation(aiAnimation* pAiAnim,const aiScene* pAiScene)
{
    shared_ptr<Animation> pAnim = make_shared<Animation>();
    pAnim->name = pAiAnim->mName.C_Str();
    pAnim->frameRate = (float)pAiAnim->mTicksPerSecond;
    pAnim->frameCount = (int)pAiAnim->mDuration + 1;

    map<string, shared_ptr<asAnimationNode>> cacheAnimNodes;

    for (uint i = 0; i < pAiAnim->mNumChannels; ++i)
    {
        auto pAnimNode = pAiAnim->mChannels[i];

        shared_ptr<asAnimationNode> asAnim = ParseAnimNode(pAnim, pAnimNode);

        pAnim->duration = std::max(pAnim->duration, asAnim->keyFrames.back().time);

        cacheAnimNodes.emplace(pAnimNode->mNodeName.C_Str(), asAnim);
    }

    ReadKeyFrameData(pAnim, pAiScene->mRootNode, cacheAnimNodes);

    return pAnim;
}

shared_ptr<asAnimationNode> AssimpMgr::ParseAnimNode(shared_ptr<Animation> pAnim, aiNodeAnim* pAnimNode)
{
    shared_ptr<asAnimationNode> node = make_shared<asAnimationNode>();
    node->aiAnimName = pAnimNode->mNodeName;

    const uint keyCount = std::max(std::max(pAnimNode->mNumPositionKeys, pAnimNode->mNumScalingKeys), pAnimNode->mNumRotationKeys);

    for (uint i = 0; i < keyCount; ++i)
    {
        KeyFrameData frameData;
        bool bFound = false;
        uint t = (uint)node->keyFrames.size();

        if (IsFloatZero((float)pAnimNode->mPositionKeys[i].mTime - (float)t))
        {
            aiVectorKey key = pAnimNode->mPositionKeys[i];
            frameData.time = (float)key.mTime;
            frameData.translation.x = key.mValue.x;
            frameData.translation.y = key.mValue.y;
            frameData.translation.z = key.mValue.z;
            bFound = true;
        }

        if (IsFloatZero((float)pAnimNode->mRotationKeys[i].mTime - (float)t))
        {
            aiQuatKey key = pAnimNode->mRotationKeys[i];
            frameData.time = (float)key.mTime;

            frameData.rotation.x = key.mValue.x;
            frameData.rotation.y = key.mValue.y;
            frameData.rotation.z = key.mValue.z;
            frameData.rotation.w = key.mValue.w;

            bFound = true;
        }

        if (IsFloatZero((float)pAnimNode->mScalingKeys[i].mTime - (float)t))
        {
            aiVectorKey key = pAnimNode->mScalingKeys[i];
            frameData.time = (float)key.mTime;
            frameData.scale.x = key.mValue.x;
            frameData.scale.y = key.mValue.y;
            frameData.scale.z = key.mValue.z;
            bFound = true;
        }

        if (bFound)
        {
            node->keyFrames.emplace_back(frameData);
        }
    }

    if (node->keyFrames.size() < pAnim->frameCount)
    {
        uint count = pAnim->frameCount - (uint)node->keyFrames.size();
        KeyFrameData keyFrame = node->keyFrames.back();
        for (uint i = 0; i < count; ++i)
        {
            node->keyFrames.emplace_back(keyFrame);
        }
    }

    return node;
}

void AssimpMgr::ReadKeyFrameData(shared_ptr<Animation> pAnim, aiNode* pNode, map<string, shared_ptr<asAnimationNode>>& cache)
{
    shared_ptr<KeyFrame> keyFrame = make_shared<KeyFrame>();
    keyFrame->boneName = pNode->mName.C_Str();

    shared_ptr<asAnimationNode> findNode = cache[pNode->mName.C_Str()];

    for (uint i = 0; i < (uint)pAnim->frameCount; ++i)
    {
        KeyFrameData frameData;

        if (nullptr == findNode)
        {
            glm::mat4 transformMat = (ConvertAiMatrixToGLM(pNode->mTransformation));
            frameData.time = (float)i;

            const glm::vec3 translation = glm::vec3(transformMat[3]);

            glm::vec3 scale;
            scale.x = glm::length(glm::vec3(transformMat[0]));
            scale.y = glm::length(glm::vec3(transformMat[1]));
            scale.z = glm::length(glm::vec3(transformMat[2]));

            glm::mat4 rotationMatrix = transformMat;
            rotationMatrix[0] = glm::normalize(rotationMatrix[0]);
            rotationMatrix[1] = glm::normalize(rotationMatrix[1]);
            rotationMatrix[2] = glm::normalize(rotationMatrix[2]);

            frameData.translation = translation;
            frameData.scale = scale;
            frameData.rotation = glm::quat{ rotationMatrix };
           
        }
        else
        {
            frameData = findNode->keyFrames[i];
        }

        keyFrame->transforms.emplace_back(frameData);
    }

    pAnim->keyFrames.emplace_back(keyFrame);

    for (uint i = 0; i < pNode->mNumChildren; ++i)
    {
        ReadKeyFrameData(pAnim, pNode->mChildren[i], cache);
    }
}

shared_ptr<MyPolygon> AssimpMgr::ProcessNodePoly(aiNode* pNode, const aiScene* pAiScene)
{
    shared_ptr<MyPolygon> pObj = make_shared<MyPolygon>();
    pObj->SetObjName(pNode->mName.C_Str());

    for (GLuint i = 0; i < pNode->mNumMeshes; ++i)
    {
        auto pAiMesh = pAiScene->mMeshes[pNode->mMeshes[i]];

        auto simpleMesh = ProcessMeshPoly(pAiMesh);
        pObj = make_shared<MyPolygon>(std::move(simpleMesh.first), std::move(simpleMesh.second));
        MyPolygon::AddMainPoly(pObj);
    }

    SetTransformation(pObj->GetTransform(), pNode);
    
    for (GLuint i = 0; i < pNode->mNumChildren; ++i)
    {
        pObj->AddChild(ProcessNodePoly(pNode->mChildren[i], pAiScene));
    }

    return pObj;
}

std::pair<vector<SimpleVertex>,vector<GLuint>> AssimpMgr::ProcessMeshPoly(aiMesh* pAiMesh)
{
    vector<SimpleVertex> vertices;
    vector<GLuint> indices;
    vertices.reserve(pAiMesh->mNumVertices);
    indices.reserve(pAiMesh->mNumFaces * 3);

    auto f = Mgr(ThreadMgr)->EnqueueTaskFuture([&]() {
        for (GLuint i = 0; i < pAiMesh->mNumVertices; ++i)
        {
            SimpleVertex v;

            v.pos.x = pAiMesh->mVertices[i].x;
            v.pos.y = pAiMesh->mVertices[i].y;
            v.pos.z = pAiMesh->mVertices[i].z;

            if (pAiMesh->mColors[0])
            {
                v.color.r = pAiMesh->mColors[0][i].r;
                v.color.g = pAiMesh->mColors[0][i].g;
                v.color.b = pAiMesh->mColors[0][i].b;
            }

            vertices.emplace_back(v);
        }
        });

    for (GLuint i = 0; i < pAiMesh->mNumFaces; ++i)
    {
        auto aiFace = pAiMesh->mFaces[i];
        for (GLuint j = 0; j < aiFace.mNumIndices; ++j)
        {
            indices.emplace_back(aiFace.mIndices[j]);
        }
    }
    f.get();
    return std::make_pair(std::move(vertices), std::move(indices));
}

shared_ptr<GameObj> AssimpMgr::ProcessNodeGameObj(string_view strShaderName_, aiNode* pNode, const aiScene* pAiScene)noexcept
{
    auto pObj = GameObj::make_obj();
    auto pMeshRenderer = pObj->AddComponent<MeshRenderer>();
    pMeshRenderer->SetShader(strShaderName_);
    pObj->SetObjName(pNode->mName.C_Str());
    
    auto res_future = make_shared<vector<std::future<shared_ptr<Mesh>>>>();
    res_future->reserve(pNode->mNumMeshes);

    for (GLuint i = 0; i < pNode->mNumMeshes; ++i)
    {
        auto pAiMesh = pAiScene->mMeshes[pNode->mMeshes[i]];

        res_future->emplace_back(Mgr(ThreadMgr)->EnqueueTaskFuture(&AssimpMgr::ProcessMesh, this, pAiMesh));

        auto pMaterial = ProcessMaterial(pAiMesh, pAiScene);

        pMeshRenderer->AddMaterial(std::move(pMaterial));
    }

    SetTransformation(pObj->GetTransform(), pNode);

    m_fpSetBuf.emplace_back(std::async(std::launch::deferred, [this, pNode, pAiScene, pMeshRenderer, res_future = std::move(res_future)]()mutable noexcept {
        for (auto& f : *res_future)
        {
            auto pMesh = f.get();
            ///*if (pAiScene->HasAnimations())
            //{
            //    ProcessBone(pMesh->GetVertices(), pNode, pAiScene);
            //}*/
            pMesh->SetBuffers();
            pMeshRenderer->AddMesh(std::move(pMesh));
        }}));

    for (GLuint i = 0; i < pNode->mNumChildren; ++i)
    {
        pObj->AddChild(ProcessNodeGameObj(strShaderName_,pNode->mChildren[i], pAiScene));
    }

    return pObj;
}

void AssimpMgr::Init()
{
    m_wait.reserve(100);
    m_fpSetBuf.reserve(1000);
}

shared_ptr<GameObj> AssimpMgr::Load(string_view _strShaderName, string_view _strModelFileName)noexcept
{
    if (auto modelData = Mgr(ResMgr)->GetRes<Model>(_strModelFileName))
    {
        auto pObj = GameObj::make_obj();
        auto m = pObj->AddComponent<MeshRenderer>();
        pObj->SetResName(_strModelFileName);
        m->SetModelData(modelData);
        m->SetShader(_strShaderName);
        return pObj;
    }
    tempName = _strModelFileName;
    unsigned int flag;
    
    flag = aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipWindingOrder;

    const aiScene* scene = m_importer.ReadFile((Mgr(PathMgr)->GetObjPath()/_strModelFileName).string().data(),
        flag);

    auto pObj = GameObj::make_obj();
    pObj->SetResName(_strModelFileName);

    auto pMeshRenderer = pObj->AddComponent<MeshRenderer>();

    pMeshRenderer->SetShader(_strShaderName);
    pMeshRenderer->SetModelData(ProcessNode(scene->mRootNode, scene));

    for (auto& f : m_wait)f.get();
    m_wait.clear();
    for (auto& fp : m_fpSetBuf) fp.get();
    m_fpSetBuf.clear();

    Mgr(ResMgr)->AddRes<Model>(_strModelFileName, pMeshRenderer->GetModel());
   // Mgr(ThreadMgr)->SetJobCount(0);
    //if (scene->HasAnimations())
    //{
    //    auto pAnimator = pObj->AddComponent<Animator>();

    //    for (uint i = 0; i < scene->mNumAnimations; ++i)
    //    {
    //        auto pAiAnim = scene->mAnimations[i];

    //        auto pAnim = ProcessAnimation(pAiAnim, scene);

    //        pAnimator->AddAnimation(pAnim->name,pAnim);
    //    }

    //   // UpdateBoneParentIndices(scene->mRootNode);

    //    for (int i = 0; i < (int)g_tempBoneVec.size(); ++i)
    //    {
    //        g_tempBoneVec[i].worldTransform = GetBoneWorldTransform(i);
    //    }

    //    pAnimator->m_BoneMap = std::move(g_tempBoneMap);
    //    pAnimator->m_vecBones = std::move(g_tempBoneVec);
    //    pAnimator->m_curAnim = pAnimator->m_mapAnim.begin()->second;

    //    boneCounter = 0;
    //}
    tempName.clear();
    return pObj;
}

shared_ptr<Model> AssimpMgr::LoadModel(string_view _strModelFileName)noexcept
{
    if (auto modelData = Mgr(ResMgr)->GetRes<Model>(_strModelFileName))
    {
        return modelData;
    }

    unsigned int flag;

    flag = aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipWindingOrder;

    const aiScene* scene = m_importer.ReadFile((Mgr(PathMgr)->GetObjPath() / _strModelFileName).string().data(),
        flag);

    auto pModel = ProcessNode(scene->mRootNode, scene);
   

    for (auto& f : m_wait)f.get();
    m_wait.clear();
    for (auto& fp : m_fpSetBuf) fp.get();
    m_fpSetBuf.clear();

    Mgr(ResMgr)->AddRes<Model>(_strModelFileName, pModel);

   // Mgr(ThreadMgr)->SetJobCount(0);
   
    return pModel;
}

shared_ptr<MyPolygon> AssimpMgr::LoadPoly(string_view _strPolyFileName)
{
    unsigned int flag;

    flag = aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipWindingOrder;

    const aiScene* scene = m_importer.ReadFile((Mgr(PathMgr)->GetObjPath() / _strPolyFileName).string().data(),
        flag);

    auto pPoly = ProcessNodePoly(scene->mRootNode, scene);
    //Mgr(ThreadMgr)->SetJobCount(0);
    return pPoly;
}

shared_ptr<GameObj> AssimpMgr::LoadAllPartsAsGameObj(string_view _strShaderName, string_view _strModelFileName)noexcept
{
    unsigned int flag;

    flag = aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipWindingOrder;

    const aiScene* scene = m_importer.ReadFile((Mgr(PathMgr)->GetObjPath() / _strModelFileName).string().data(),
        flag);

    auto pObj = ProcessNodeGameObj(_strShaderName,scene->mRootNode, scene);


    for (auto& f : m_wait)f.get();
    m_wait.clear();
    for (auto& fp : m_fpSetBuf) fp.get();
    m_fpSetBuf.clear();

    return pObj;
}


void UpdateBoneParentIndices(aiNode* pNode)
{
    if (g_tempBoneMap.find(pNode->mName.C_Str()) != g_tempBoneMap.end())
    {
        uint boneIndex = g_tempBoneMap[pNode->mName.C_Str()];

        if (pNode->mParent && g_tempBoneMap.find(pNode->mParent->mName.C_Str()) != g_tempBoneMap.end())
        {
            g_tempBoneVec[boneIndex].parentIndex = g_tempBoneMap[pNode->mParent->mName.C_Str()];
            g_tempBoneVec[boneIndex].worldTransform = g_tempBoneVec[g_tempBoneVec[boneIndex].parentIndex].worldTransform * g_tempBoneVec[boneIndex].worldTransform;
        }
    }
}

glm::mat4 GetBoneWorldTransform(const int idx)
{
    BoneInfo& bone = g_tempBoneVec[idx];
    return bone.parentIndex < 0 ? bone.offsetMat : GetBoneWorldTransform(bone.parentIndex) * bone.offsetMat;
}