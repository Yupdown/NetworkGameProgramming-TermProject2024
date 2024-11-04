#include "pch.h"
#include "Transform.h"

const glm::quat Transform::defaultQuat = glm::angleAxis(glm::radians(0.0f), glm::vec3{0.0f, 1.0f, 0.0f});

Transform::Transform()
	:Component{COMPONENT_TYPE::TRANSFORM}
{
}

Transform::~Transform()
{
}
