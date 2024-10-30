#pragma once
#include "pch.h"

struct KeyFrameData
{
    float time;
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
};

struct KeyFrame
{
    string boneName;
    vector<KeyFrameData> transforms;
};

struct Animation
{
    Animation();
    ~Animation();
    string name;
    uint frameCount = 0;
    float frameRate =0.f;
    float duration =0.f;
   vector<shared_ptr<KeyFrame>> keyFrames;
};

// Cache
struct asAnimationNode
{
    aiString aiAnimName;
    vector<KeyFrameData> keyFrames;
};