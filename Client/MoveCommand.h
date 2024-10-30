#pragma once
#include "pch.h"
#include "Command.h"
#include "GameObj.h"
#include "Transform.h"
#include "TimeMgr.h"

#define MOVE_COMMAND_SPEED 5.f

class MoveFront :public Command {
public:
	void Execute(const KEY_STATE _eState, const shared_ptr<GameObj>& _pObj)override {
		if (KEY_STATE::HOLD == _eState)
		{
			const auto pTrans = _pObj->GetTransform();
			pTrans->AddFrontBack(MOVE_COMMAND_SPEED * DT);
		}
	}
};
class MoveBack :public Command {
public:
	void Execute(const KEY_STATE _eState, const shared_ptr<GameObj>& _pObj)override {
		if (KEY_STATE::HOLD == _eState)
		{
			const auto pTrans = _pObj->GetTransform();
			pTrans->AddFrontBack(-MOVE_COMMAND_SPEED * DT);
		}
	}
};
class MoveLeft :public Command {
public:
	void Execute(const KEY_STATE _eState, const shared_ptr<GameObj>& _pObj)override {
		if (KEY_STATE::HOLD == _eState)
		{
			const auto pTrans = _pObj->GetTransform();
			pTrans->AddLeftRight(-MOVE_COMMAND_SPEED * DT);
		}
	}
};
class MoveRight :public Command {
public:
	void Execute(const KEY_STATE _eState, const shared_ptr<GameObj>& _pObj)override {
		if (KEY_STATE::HOLD == _eState)
		{
			const auto pTrans = _pObj->GetTransform();
			pTrans->AddLeftRight(MOVE_COMMAND_SPEED * DT);
		}
	}
};
class MoveUp :public Command {
public:
	void Execute(const KEY_STATE _eState, const shared_ptr<GameObj>& _pObj)override {
		if (KEY_STATE::HOLD == _eState)
		{
			const auto pTrans = _pObj->GetTransform();
			pTrans->AddUpDown(MOVE_COMMAND_SPEED * DT);
		}
	}
};
class MoveDown :public Command {
public:
	void Execute(const KEY_STATE _eState, const shared_ptr<GameObj>& _pObj)override {
		if (KEY_STATE::HOLD == _eState)
		{
			const auto pTrans = _pObj->GetTransform();
			pTrans->AddUpDown(-MOVE_COMMAND_SPEED * DT);
		}
	}
};
class MoveLeftRotate : public Command {
public:
	void Execute(const KEY_STATE _eState, const shared_ptr<GameObj>& _pObj)override {
		if (KEY_STATE::HOLD == _eState)
		{
			const auto pTrans = _pObj->GetTransform();
			pTrans->AddWorldRotation(-70.f * DT, Y_AXIS);
		}
	}
};
class MoveRightRotate : public Command {
public:
	void Execute(const KEY_STATE _eState, const shared_ptr<GameObj>& _pObj)override {
		if (KEY_STATE::HOLD == _eState)
		{
			const auto pTrans = _pObj->GetTransform();
			pTrans->AddWorldRotation(70.f * DT, Y_AXIS);
		}
	}
};