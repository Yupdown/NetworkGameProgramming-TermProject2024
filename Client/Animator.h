#pragma once
#include "Component.h"

struct Animation;

class Animator :
    public Component
{
    static vector<glm::mat4> g_vecBoneTransform;
    friend class AssimpMgr;
    COMP_CLONE(Animator)
private:
    unordered_map<string, shared_ptr<Animation>> m_mapAnim;
    vector<BoneInfo> m_vecBones;
    unordered_map<string, int> m_BoneMap;
    shared_ptr<Animation> m_curAnim;
public:
    Animator();
    ~Animator();
public:
    void AddAnimation(string_view _strAnimName, shared_ptr<Animation> pAnim) { m_mapAnim.emplace(_strAnimName.data(), std::move(pAnim)); }

    virtual void Update() {}
    virtual void LateUpdate() {}
    virtual void LastUpdate() {}

    virtual void FinalUpdate() {}

    virtual void PreRender()const;
    virtual void Render()const {}

    glm::mat4 CalculTransform(uint boneIdx, vector<KeyFrameData>& keyFrameDatas)const;

    virtual void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath) override
    {
        writer.String("Animator");
    }
};


