#pragma once
#include "Component.h"

class State;

class FSM
	:public Component
{
public:
	void Update(const float DT)override;
	void AddState(std::shared_ptr<State> state);
	void Start(uint8_t state){
		// ã�Ƽ� ������ �ÿ��ϰ� ������ !
		m_curState = m_mapState.find(state)->second.get();
	}
public:
	std::unordered_map<uint8_t, std::shared_ptr<State>> m_mapState;
	State* m_curState = nullptr;
	//ServerCore::S_ptr<class ClientSession> target;
};

