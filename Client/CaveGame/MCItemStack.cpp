#include "pch.h"
#include "MCItemStack.h"
#include "MCItem.h"

MCItemStack::MCItemStack()
{
	m_item = nullptr;
	m_stackSize = 0;
}

MCItemStack::MCItemStack(MCItem* item, int stack_size)
{
	m_item = item;
	m_stackSize = std::min(stack_size, m_item->GetMaxStacksize());
}

MCItemStack::~MCItemStack()
{

}

MCItem* MCItemStack::GetItem() const
{
	return m_item;
}

int MCItemStack::GetStackSize() const
{
	return m_stackSize;
}

bool MCItemStack::Null() const
{
	return m_item == nullptr;
}

int MCItemStack::StackUp(int amount)
{
	m_stackSize += amount;
	int max_size = m_item->GetMaxStacksize();

	if (m_stackSize > max_size)
	{
		int delta = m_stackSize - max_size;
		m_stackSize = max_size;
		return delta;
	}
	return 0;
}

void MCItemStack::Apply(MCItemStack& dest, MCItemStack& src)
{
	bool flag = false;

	if (!dest.Null() && !src.Null())
		flag = dest.GetItem() == src.GetItem();

	if (flag)
	{
		int size = dest.StackUp(src.GetStackSize());
		src.m_stackSize = size;
	}
	else
		std::swap(dest, src);
}

void MCItemStack::AddStack(MCItemStack& dest, int amount)
{
	dest.StackUp(amount);
}
