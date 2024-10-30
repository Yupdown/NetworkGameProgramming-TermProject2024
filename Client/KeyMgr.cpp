#include "pch.h"
#include "KeyMgr.h"
#include "Core.h"

KeyMgr::KeyMgr()
{
}

KeyMgr::~KeyMgr()
{
}

void KeyMgr::Init()
{
	m_pWinInfo = Mgr(Core)->GetWinInfo();
	m_mapKey.reserve(1000);
}

void KeyMgr::Update()noexcept
{
	if (glfwGetWindowAttrib(m_pWinInfo, GLFW_FOCUSED))
	{
		for (auto& [VKval, keyinfo] : m_mapKey)
		{
			const auto glKeyCheckFunc = GLFW_MOUSE_BUTTON_LEFT == VKval
				|| GLFW_MOUSE_BUTTON_RIGHT == VKval
				|| GLFW_MOUSE_BUTTON_MIDDLE == VKval
				? glfwGetMouseButton : glfwGetKey;
			if (glKeyCheckFunc(m_pWinInfo, VKval) == GLFW_PRESS)
			{
				if (keyinfo.bPrevPush)
				{
					keyinfo.eState = KEY_STATE::HOLD;
				}
				else
				{
					keyinfo.eState = KEY_STATE::TAP;
				}
				keyinfo.bPrevPush = true;
			}
			else
			{
				if (keyinfo.bPrevPush)
				{
					keyinfo.eState = KEY_STATE::AWAY;
				}
				else
				{
					keyinfo.eState = KEY_STATE::NONE;
				}
				keyinfo.bPrevPush = false;
			}
		}
		double xpos, ypos;
		glfwGetCursorPos(m_pWinInfo, &xpos, &ypos);
		m_vPrevMousePos = m_vMousePos;
		m_vMousePos.x = static_cast<float>(xpos);
		m_vMousePos.y = static_cast<float>(ypos);
	}
	else
	{
		for (auto& [VKval, keyinfo] : m_mapKey)
		{
			keyinfo.bPrevPush = false;

			if (KEY_STATE::TAP == keyinfo.eState || KEY_STATE::HOLD == keyinfo.eState)
			{
				keyinfo.eState = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == keyinfo.eState)
			{
				keyinfo.eState = KEY_STATE::NONE;
			}
		}
	}
}
