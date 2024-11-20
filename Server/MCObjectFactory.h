#pragma once
#include "pch.h"

class Object;

struct MCObjectBuilder
{
	glm::vec3 pos;
	uint32_t obj_id;
};

class MCObjectFactory
{
public:
	MCObjectFactory() = delete;
	~MCObjectFactory() = delete;
public:
	static S_ptr<Object> CreateMonster(MCObjectBuilder& b_);
};

