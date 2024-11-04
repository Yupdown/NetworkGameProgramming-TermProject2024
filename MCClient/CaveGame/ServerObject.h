#pragma once

#include <GameObj.h>

class ServerObject : public GameObj
{
public:
	ServerObject(unsigned int id);
	~ServerObject() noexcept;

private:
	unsigned int m_id = 0;
};