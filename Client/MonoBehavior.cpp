#include "pch.h"
#include "MonoBehavior.h"

MonoBehavior::MonoBehavior()
	:Component{COMPONENT_TYPE::MONO_BEHAVIOR}
{
}

MonoBehavior::MonoBehavior(string_view _behaviorName)
	:Component{COMPONENT_TYPE::MONO_BEHAVIOR}
{
	SetResName(_behaviorName);
}

MonoBehavior::~MonoBehavior()
{
}
