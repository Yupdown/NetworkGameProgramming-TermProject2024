#pragma once
#include "pch.h"

class Component;

class Object
{
public:

private:
	std::vector<std::unique_ptr<Component>> m_mapComp;
};

