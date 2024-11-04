#pragma once
#include "pch.h"

struct BoneInfo
{
	string name;
	glm::mat4 offsetMat;
	glm::mat4 worldTransform;
	int index = -1;
	int parentIndex =-1;
};