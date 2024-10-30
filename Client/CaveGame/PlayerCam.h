#pragma once
#include "GameObj.h"

class PlayerCam
	:public GameObj
{
private:
public:
	PlayerCam();
	~PlayerCam();

	void ReverseCam()noexcept;
};

