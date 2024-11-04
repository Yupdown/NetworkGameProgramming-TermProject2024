#include "pch.h"
#include "EventMgr.h"
#include "CoRoutine.hpp"
#include "RayCaster.h"
#include "KeyMgr.h"
#include "DoubleLockQueue.hpp"

std::future<void> g_resetMemPool;

EventMgr::EventMgr()
{
}

EventMgr::~EventMgr()
{
}

void EventMgr::Update()
{
	m_vecInternalGameEventBuffer.swap(m_vecGameEvent);

	for (auto& eve : m_vecInternalGameEventBuffer)
	{
		std::visit(m_eveHandler, std::move(eve));
	}

	m_vecInternalGameEventBuffer.clear();

	for (auto& eve : m_vecGameEvent)
	{
		std::visit(m_eveHandler, std::move(eve));
	}

	for (auto iter = m_listCoTask.begin(); iter != m_listCoTask.end();)
	{
		if (iter->done())
		{
			iter = m_listCoTask.erase(iter);
		}
		else
		{
			iter->resume();
			++iter;
		}
	}

	for (auto iter = m_mapCoTask.begin(); iter != m_mapCoTask.end();)
	{
		if (iter->second.done())
		{
			iter = m_mapCoTask.erase(iter);
		}
		else
		{
			iter->second.resume();
			++iter;
		}
	}

	m_vecInternalEventBuffer.swap(m_vecEvent);

	for (const auto& eve : m_vecInternalEventBuffer)
	{
		eve();
	}

	m_vecInternalEventBuffer.clear();

	for (const auto& eve : m_vecEvent)
	{
		eve();
	}

	{
		std::lock_guard<SpinLock> lock{ m_spinLockForDangerEvent };
		m_vecInternalEventBuffer.swap(m_vecEventForNeedLock);
	}

	for (const auto& eve : m_vecInternalEventBuffer)
	{
		eve();
	}

	m_vecInternalEventBuffer.clear();

	for (const auto& eve : m_vecEvent)
	{
		eve();
	}

	m_vecEvent.clear();
	m_vecGameEvent.clear();
	m_vecDeadObj.clear();

	if (m_sceneChangeFp)
	{
		m_sceneChangeFp();
		m_sceneChangeFp = nullptr;
	}
}

void EventMgr::Init()
{
	m_vecEvent.reserve(100);
	m_vecGameEvent.reserve(100);
	m_mapCoTask.reserve(100);
}

void EventMgr::AddCoRoutine(string_view _strName, CoRoutine&& _coTask)
{
	m_mapCoTask.emplace(_strName, std::move(_coTask));
}

void EventMgr::AddCoRoutine(CoRoutine&& _coTask)
{
	m_listCoTask.emplace_back(std::move(_coTask));
}

void EventMgr::AddDeadObj(shared_ptr<GameObj>&& pDeadObj_)
{
	m_vecDeadObj.emplace_back(std::move(pDeadObj_));
}

void EventMgr::CheckMemPool() const noexcept
{
	if (DoubleLockQueue<std::function<void(void)>>::g_memPool.isNeedReset())
	{
		g_resetMemPool = Mgr(ThreadMgr)->EnqueueTaskFuture([]()noexcept {DoubleLockQueue<std::function<void(void)>>::g_memPool.checkAndResetIfNeeded(); });
	}
}
