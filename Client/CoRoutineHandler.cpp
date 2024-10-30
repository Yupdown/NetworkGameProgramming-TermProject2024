#include "pch.h"
#include "CoRoutineHandler.h"

CoRoutineHandler::CoRoutineHandler()
	:Component{ COMPONENT_TYPE::COROUTINE_HANDLER }
{
}

CoRoutineHandler::~CoRoutineHandler()
{
}

void CoRoutineHandler::Update()
{
	for (auto iter = m_listCoTask[etoi(UPDATE_TYPE::UPDATE)].begin(); iter != m_listCoTask[etoi(UPDATE_TYPE::UPDATE)].end();)
	{
		if (iter->done())
		{
			iter = m_listCoTask[etoi(UPDATE_TYPE::UPDATE)].erase(iter);
		}
		else
		{
			iter->resume();
			++iter;
		}
	}
	for (auto iter = m_mapCoTask[etoi(UPDATE_TYPE::UPDATE)].begin(); iter != m_mapCoTask[etoi(UPDATE_TYPE::UPDATE)].end();)
	{
		if (iter->second.done())
		{
			iter = m_mapCoTask[etoi(UPDATE_TYPE::UPDATE)].erase(iter);
		}
		else
		{
			iter->second.resume();
			++iter;
		}
	}
}

void CoRoutineHandler::LateUpdate()
{
	for (auto iter = m_listCoTask[etoi(UPDATE_TYPE::LATE_UPDATE)].begin(); iter != m_listCoTask[etoi(UPDATE_TYPE::LATE_UPDATE)].end();)
	{
		if (iter->done())
		{
			iter = m_listCoTask[etoi(UPDATE_TYPE::LATE_UPDATE)].erase(iter);
		}
		else
		{
			iter->resume();
			++iter;
		}
	}
	for (auto iter = m_mapCoTask[etoi(UPDATE_TYPE::LATE_UPDATE)].begin(); iter != m_mapCoTask[etoi(UPDATE_TYPE::LATE_UPDATE)].end();)
	{
		if (iter->second.done())
		{
			iter = m_mapCoTask[etoi(UPDATE_TYPE::LATE_UPDATE)].erase(iter);
		}
		else
		{
			iter->second.resume();
			++iter;
		}
	}
}

void CoRoutineHandler::LastUpdate()
{
	for (auto iter = m_listCoTask[etoi(UPDATE_TYPE::LAST_UPDATE)].begin(); iter != m_listCoTask[etoi(UPDATE_TYPE::LAST_UPDATE)].end();)
	{
		if (iter->done())
		{
			iter = m_listCoTask[etoi(UPDATE_TYPE::LAST_UPDATE)].erase(iter);
		}
		else
		{
			iter->resume();
			++iter;
		}
	}
	for (auto iter = m_mapCoTask[etoi(UPDATE_TYPE::LAST_UPDATE)].begin(); iter != m_mapCoTask[etoi(UPDATE_TYPE::LAST_UPDATE)].end();)
	{
		if (iter->second.done())
		{
			iter = m_mapCoTask[etoi(UPDATE_TYPE::LAST_UPDATE)].erase(iter);
		}
		else
		{
			iter->second.resume();
			++iter;
		}
	}
}

void CoRoutineHandler::FinalUpdate()
{
	for (auto iter = m_listCoTask[etoi(UPDATE_TYPE::FINAL_UPDATE)].begin(); iter != m_listCoTask[etoi(UPDATE_TYPE::FINAL_UPDATE)].end();)
	{
		if (iter->done())
		{
			iter = m_listCoTask[etoi(UPDATE_TYPE::FINAL_UPDATE)].erase(iter);
		}
		else
		{
			iter->resume();
			++iter;
		}
	}
	for (auto iter = m_mapCoTask[etoi(UPDATE_TYPE::FINAL_UPDATE)].begin(); iter != m_mapCoTask[etoi(UPDATE_TYPE::FINAL_UPDATE)].end();)
	{
		if (iter->second.done())
		{
			iter = m_mapCoTask[etoi(UPDATE_TYPE::FINAL_UPDATE)].erase(iter);
		}
		else
		{
			iter->second.resume();
			++iter;
		}
	}
}
