#pragma once
#include "Component.h"

class EnderDragon
	:public Component
{
private:
	static glm::vec3 bezier(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float t) {
		return (1 - t) * (1 - t) * p0 +
			2 * (1 - t) * t * p1 +
			t * t * p2;
	}
public:
	void Init();
	void Update(const float DT)override;
private:
	std::vector<glm::vec3> m_randomPoints;

	float m_bezierT = 0.f;
	float m_accTime = 0.f;

	glm::vec3 p0, p1, p2;
};

