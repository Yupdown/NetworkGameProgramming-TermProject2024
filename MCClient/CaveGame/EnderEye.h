#pragma once

#include "pch.h"
#include "GameObj.h"

class MeshRenderer;
class Material;
class EnderEye : public GameObj
{
public:
	EnderEye();
	~EnderEye() noexcept;

	void Start() override;
	void Update() override;

	void SetMoveSequence(const glm::vec3& beginPos, const glm::vec3& endPos);

private:
	shared_ptr<MeshRenderer> renderer;
	shared_ptr<Material> material;

	glm::vec3 m_beginPos;
	glm::vec3 m_endPos;

	float m_fTimer = 0.0f;
};