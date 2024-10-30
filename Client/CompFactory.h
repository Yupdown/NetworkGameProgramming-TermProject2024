#pragma once

class Component;

class CompFactory
{
public:
	static shared_ptr<Component> CreateComponent(COMPONENT_TYPE _eType);
};

