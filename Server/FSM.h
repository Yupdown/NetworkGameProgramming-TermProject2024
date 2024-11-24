#pragma once
#include "Component.h"

class State;

class FSM
	:public Component
{
public:
	void Update(const float DT)override;
	void AddState(State* const state);
	void Start(uint8_t state){
		// 찾아서 없으면 시원하게 터지자 !
		m_curState = m_mapState.find(state)->second.get();
	}
public:
	class EntityMovement* m_movement;
	std::unordered_map<uint8_t, std::unique_ptr<State>> m_mapState;
	State* m_curState = nullptr;
	S_ptr<class Object> target;
};

