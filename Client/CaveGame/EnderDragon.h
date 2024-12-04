#pragma once
#include "ServerObject.h"

class EnderDragon
	: public ServerObject
{
public:
	EnderDragon();

	void Start() override;

	void OnObjectDamaged(int value) override;
private:
};

