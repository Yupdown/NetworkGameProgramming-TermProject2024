#include "pch.h"
#include "PlayerCamScript.h"
#include "GameObj.h"
#include "Camera.h"
#include "Transform.h"
#include "AssimpMgr.h"
#include "GameObj.h"

PlayerCamScript::PlayerCamScript()
{
}

PlayerCamScript::~PlayerCamScript()
{
}

void PlayerCamScript::Start()
{
	
}

void PlayerCamScript::Update()
{
	const auto pPlayer = GetGameObj()->GetParentGameObj().lock();
}
