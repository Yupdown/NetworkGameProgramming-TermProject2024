#include "pch.h"
#include "MCItemContainer.h"

MCItemContainer::MCItemContainer()
{
	m_itemStack = MCItemStack();
}

MCItemContainer::~MCItemContainer()
{

}

MCItemStack MCItemContainer::GetItemStack() const
{
	return m_itemStack;
}

bool MCItemContainer::Blank() const
{
	return m_itemStack.Null();
}

MCItemStack MCItemContainer::Apply(MCItemStack item_stack)
{
	MCItemStack::Apply(m_itemStack, item_stack);

	return item_stack;
}

void MCItemContainer::Apply(int amount)
{
	MCItemStack::AddStack(m_itemStack, amount);
}