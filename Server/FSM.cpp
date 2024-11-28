#include "pch.h"
#include "FSM.h"
#include "State.h"
#include "Object.h"

void FSM::Update(const float DT)
{
	if (m_curState)
	{
		const MON_STATE cur_state = m_curState->m_state;
		const MON_STATE next_state = m_curState->Update(&GetOwner()->GetPosInfo(), DT);

		if (cur_state != next_state)
		{
			m_curState->ExitState(DT);
			// 없으면 걍 터지도록 함
			m_curState = m_mapState.find(next_state)->second.get();
			m_curState->EnterState(DT);
		}
	}
}

void FSM::AddState(State* const state)
{
	state->m_fsm = this;
	m_mapState.emplace(state->m_state, (state));
}
