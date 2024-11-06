#include "pch.h"
#include "Animator.h"

vector<glm::mat4> Animator::g_vecBoneTransform;

Animator::Animator()
	: Component{ COMPONENT_TYPE::ANIMATOR }
{
	g_vecBoneTransform.resize(100, glm::mat4{ 1.f });
}

Animator::~Animator()
{
}

void Animator::PreRender() const
{
	if (!m_curAnim)
		return;
	//const uint curFrameIdx = m_curAnim->frameCount;
	static float acc = 0.f;
	static uint curFrameIdx = 0;
	acc += DT;

	if (acc >= 0.016f)
	{
		curFrameIdx = (curFrameIdx + 1) % m_curAnim->frameCount;

		acc = 0.f;
	}

	int cnt = 0;

	for (auto& curKeyFrame : m_curAnim->keyFrames)
	{

		auto iter = m_BoneMap.find(curKeyFrame->boneName);

		if (m_BoneMap.end() == iter)continue;

		const uint boneIdx = iter->second;

		auto& bone = m_vecBones[boneIdx];


		auto& keyFrameTransform = curKeyFrame->transforms[curFrameIdx];

		glm::mat4 T, R, S;

		T = glm::translate(glm::mat4{ 1.f }, keyFrameTransform.translation);

		R = glm::mat4_cast(keyFrameTransform.rotation);

		S = glm::scale(glm::mat4{ 1.f }, keyFrameTransform.scale);

		g_vecBoneTransform[boneIdx] = (bone.worldTransform) *S* R*T;
	}
		
	int t = 0;

	glGetIntegerv(GL_CURRENT_PROGRAM, &t);

	//GLint boneLocation = glGetUniformLocation(t, "uBoneTransforms");

	for (int i = 0; i < 100; ++i)
	{
		const string loc = std::format("uBoneTransforms[{}]", i);
		GLint boneLocation = glGetUniformLocation(t, loc.c_str());
		glUniformMatrix4fv(boneLocation, 1, GL_FALSE, glm::value_ptr(g_vecBoneTransform[i]));
	}
}

glm::mat4 Animator::CalculTransform(uint boneIdx, vector<KeyFrameData>& keyFrameDatas)const
{
	if (boneIdx == -1) // 루트를 의미하는 값; 이 값은 프로젝트에 따라 다를 수 있습니다.
		return glm::mat4(1.0f);

	const auto& bone = m_vecBones[boneIdx];
	//const auto& keyFrameTransform = 

	glm::mat4 T = glm::translate(glm::mat4{ 1.f }, keyFrameDatas[boneIdx].translation);
	glm::mat4 R = glm::mat4_cast(keyFrameDatas[boneIdx].rotation);
	glm::mat4 S = glm::scale(glm::mat4{ 1.f }, keyFrameDatas[boneIdx].scale);

	glm::mat4 localTransform = T * R * S;

	glm::mat4 parentTransform = CalculTransform(bone.parentIndex, keyFrameDatas); // 부모 본의 변환을 재귀적으로 계산합니다.

	return parentTransform * localTransform;
}
