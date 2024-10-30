#include "pch.h"
#include "PlayerMoveScript.h"
#include "KeyMgr.h"
#include "TimeMgr.h"
#include "Transform.h"

PlayerMoveScript::PlayerMoveScript()
{
}

PlayerMoveScript::~PlayerMoveScript()
{
}

void PlayerMoveScript::Update()
{
	const auto pPlayerTrans = GetTransform();

	if (KEY_HOLD(GLFW_KEY_A))
	{
		pPlayerTrans->AddLeftRight(-m_fMoveSpeed * DT);
	}
	if (KEY_HOLD(GLFW_KEY_W))
	{
		pPlayerTrans->AddFrontBack(m_fMoveSpeed * DT);
	}
	if (KEY_HOLD(GLFW_KEY_S))
	{
		pPlayerTrans->AddFrontBack(-m_fMoveSpeed * DT);
	}
	if (KEY_HOLD(GLFW_KEY_D))
	{
		pPlayerTrans->AddLeftRight(m_fMoveSpeed * DT);
	}
	if (KEY_HOLD(GLFW_KEY_Q))
	{
		pPlayerTrans->AddWorldRotation(-50.f * DT, Y_AXIS);
	}
	if (KEY_HOLD(GLFW_KEY_E))
	{
		pPlayerTrans->AddWorldRotation(50.f * DT, Y_AXIS);
	}
	if (KEY_HOLD(GLFW_KEY_UP))
	{
		pPlayerTrans->AddUpDown(m_fMoveSpeed * DT);
	}
	if (KEY_HOLD(GLFW_KEY_DOWN))
	{
		pPlayerTrans->AddUpDown(-m_fMoveSpeed * DT);
	}
}
