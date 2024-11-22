#pragma once

class MCItem;

struct MCItemStack
{
private:
	MCItem* m_item;
	int m_stackSize;

public:
	MCItemStack();
	MCItemStack(MCItem* item, int stack_size);
	~MCItemStack();

	MCItem* GetItem() const;
	int GetStackSize() const;
	bool Null() const;

	int StackUp(int amount);
	static void Apply(MCItemStack& dest, MCItemStack& src);
	static void AddStack(MCItemStack& dest, int amount);
};