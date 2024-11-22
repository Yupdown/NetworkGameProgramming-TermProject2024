#pragma once

#include "MCItemStack.h"

class MCItemContainer
{
private:
	MCItemStack m_itemStack;

public:
	MCItemContainer();
	~MCItemContainer();

	MCItemStack GetItemStack() const;
	bool Blank() const;

	MCItemStack Apply(MCItemStack itemStack);
	void Apply(int amount);
};