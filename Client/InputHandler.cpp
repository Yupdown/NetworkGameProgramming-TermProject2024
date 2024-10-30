#include "pch.h"
#include "InputHandler.h"
#include "KeyMgr.h"
#include "Command.h"

InputHandler::InputHandler()
	:Component{COMPONENT_TYPE::INPUT_HANDLER}
{
}

InputHandler::~InputHandler()
{
}

void InputHandler::Update()
{
	const auto pGameObj = GetGameObj();

	for (const auto& [VKval, command] : m_mapCommand)
	{
		command->Execute(Mgr(KeyMgr)->GetKeyState(VKval),pGameObj);
	}
}