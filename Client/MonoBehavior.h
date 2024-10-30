#pragma once
#include "Component.h"

class GameObj;

class MonoBehavior
	:public Component
{
	virtual void FinalUpdate()override final {};
	virtual void PreRender()const override final {};
	virtual void Render()const override final {};
public:
	//virtual shared_ptr<Component> Comp_Clone()const override = 0;
	
	MonoBehavior();
	MonoBehavior(string_view _behaviorName);
	virtual ~MonoBehavior();

	virtual void Update() {}
	virtual void LateUpdate(){}
	virtual void LastUpdate() {}

public:
	virtual void OnCollisionEnter(shared_ptr<GameObj> _pOtherObj){}
	virtual void OnCollisionStay(shared_ptr<GameObj> _pOtherObj) {}
	virtual void OnCollisionExit(shared_ptr<GameObj> _pOtherObj) {}

public:
	COMP_CLONE(MonoBehavior)
};

