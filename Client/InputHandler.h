#pragma once
#include "Component.h"

class Command;

class InputHandler
	:public Component
{
	COMP_CLONE(InputHandler)
private:
	unordered_map<int, shared_ptr<Command>> m_mapCommand;
public:
	InputHandler();
	~InputHandler();
	void Update()override;
	void SetCommand(const int _VKval, shared_ptr<Command> _pCommand) { m_mapCommand.emplace(_VKval, std::move(_pCommand)); }

	virtual void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath) override
	{
		writer.String("InputHandler");
	}

};

