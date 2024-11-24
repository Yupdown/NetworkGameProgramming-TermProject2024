#pragma once

enum MON_STATE
{
	PATROL,
	CHASE,


	END,
};

class FSM;
class EntityMovement;

static constexpr float ATK_DIST = 3.f;
static constexpr float ATK_TIME = 1.f;

class State
{
public:
	virtual MON_STATE Update(EntityMovement* const move_system, const float DT) = 0;
	virtual void EnterState(const float DT){}
	virtual void ExitState(const float DT) {}
public:
	FSM* m_fsm = nullptr;
	MON_STATE m_state;
};

class Patrol
	:public State
{
public:
	Patrol() { m_state = PATROL; }
public:
	virtual MON_STATE Update(EntityMovement* const move_system, const float DT) override;
	virtual void EnterState(const float DT)override;
	virtual void ExitState(const float DT)override;
public:
	float m_accTime = 0.f;
};

class Chase
	:public State
{
	
public:
	Chase() { m_state = CHASE; }
public:
	virtual MON_STATE Update(EntityMovement* const move_system, const float DT)override;
	virtual void EnterState(const float DT) override;
	virtual void ExitState(const float DT) override;
public:
};
