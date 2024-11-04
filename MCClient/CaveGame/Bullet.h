#pragma once
#include "GameObj.h"

class Bullet
	:public GameObj
{
private:
	glm::vec3 m_bulletDir = glm::vec3{};
public:
	Bullet(const glm::vec3 initPos,const glm::vec3 initDir);
	~Bullet();

	void Update()override;
};

