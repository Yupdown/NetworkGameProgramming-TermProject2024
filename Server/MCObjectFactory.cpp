#include "pch.h"
#include "MCObjectFactory.h"
#include "MCWorld.h"
#include "Object.h"
#include "EntityMovement.h"
#include "FSM.h"
#include "State.h"

S_ptr<Object> MCObjectFactory::CreateMonster(MCObjectBuilder& b_)
{
	auto mon = std::make_shared<Object>();
	mon->SetObjectType(MC_OBJECT_TYPE::MONSTER);
	
	const auto fsm = mon->AddComp<FSM>(new FSM);
	fsm->AddState(new Patrol);
	fsm->AddState(new Chase);
	fsm->Start(MON_STATE::PATROL);

	const auto m = mon->AddComp<EntityMovement>(new EntityMovement);
	mon->SetEntityMovement(m);
	fsm->m_movement = mon->GetEntityMovement();
	mon->SetPos(b_.pos);
	mon->GetEntityMovement()->current_position = b_.pos;
	
	return mon;
}
