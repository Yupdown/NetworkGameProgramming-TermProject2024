#include "pch.h"
#include "MCObjectFactory.h"
#include "MCWorld.h"
#include "Object.h"
#include "EntityMovement.h"

S_ptr<Object> MCObjectFactory::CreateMonster(MCObjectBuilder& b_)
{
	auto mon = std::make_shared<Object>();
	mon->SetObjectType(MC_OBJECT_TYPE::MONSTER);
	mon->GetEntityMovement()->current_position = b_.pos;
	return mon;
}
