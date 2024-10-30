#pragma once

class GameObj;

class Command
{
public:
	Command();
	virtual ~Command();
	virtual void Execute(const KEY_STATE _eState, const shared_ptr<GameObj>& _pObj) = 0;
};

