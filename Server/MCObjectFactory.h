#pragma once
#include "pch.h"

class Object;

struct MCObjectBuilder
{
	glm::vec3 pos;
};

struct MCItemBuilder
	:public MCObjectBuilder
{
	uint8_t item_id;
};

struct EnderDragonBuilder
	:public MCObjectBuilder
{
	
};

class MCObjectFactory
{
public:
	MCObjectFactory() = delete;
	~MCObjectFactory() = delete;
public:
	static S_ptr<Object> CreateMonster(MCObjectBuilder& b_);
	static S_ptr<Object> CreateItem(MCItemBuilder& b_);
	static S_ptr<Object> CreateEnderDragon(EnderDragonBuilder& b_);
};

