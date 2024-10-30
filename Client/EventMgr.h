#pragma once
#include "EventHandler.h"

class CoRoutine;
class GameObj;

class EventMgr
	:public Singleton<EventMgr>
{
	friend class Singleton;
	EventMgr();
	~EventMgr();
private:
	vector<function<void(void)>> m_vecEvent;
	vector<GameEvent> m_vecGameEvent;
	unordered_map<string, CoRoutine> m_mapCoTask;
	list<CoRoutine> m_listCoTask;
	EventHandler m_eveHandler = {};
	vector<shared_ptr<GameObj>> m_vecDeadObj;

	vector<function<void(void)>> m_vecInternalEventBuffer;
	vector<GameEvent> m_vecInternalGameEventBuffer;

	SpinLock m_spinLockForDangerEvent;
	vector<function<void(void)>> m_vecEventForNeedLock;

	function<void(void)> m_sceneChangeFp;
public:
	void SetSceneChangeFp(function<void(void)>&& sceneChangeFp) { m_sceneChangeFp = std::move(sceneChangeFp); }
	void Init();
	void Update();
	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	void AddEvent(Func&& fp, Args&&... args) { m_vecEvent.emplace_back([fp = std::forward<Func>(fp), ...args = std::forward<Args>(args)]()mutable {std::invoke(std::forward<Func>(fp), std::forward<Args>(args)...); }); }
	void AddEvent(GameEvent _eveStruct) { m_vecGameEvent.emplace_back(std::move(_eveStruct)); }
	void AddCoRoutine(string_view _strName, CoRoutine&& _coTask);
	void AddCoRoutine(CoRoutine&& _coTask);

	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	void AddEventNeedLock(Func&& fp, Args&&... args)noexcept
	{
		std::lock_guard<SpinLock> lock{ m_spinLockForDangerEvent };
		m_vecEventForNeedLock.emplace_back([fp = std::forward<Func>(fp), ...args = std::forward<Args>(args)]()mutable noexcept{std::invoke(std::forward<Func>(fp), std::forward<Args>(args)...); });
	}

	bool FindCoRoutine(string_view _strKey)const
	{
		return m_mapCoTask.contains(_strKey.data());
	}

	bool DestroyCoRoutine(string_view _strKey)
	{
		const bool bRes = FindCoRoutine(_strKey);
		m_mapCoTask.erase(_strKey.data());
		return bRes;
	}

	void AddDeadObj(shared_ptr<GameObj>&& pDeadObj_);

	void Reset()
	{
		m_vecEvent.clear();
		m_vecGameEvent.clear();
		m_mapCoTask.clear();
		m_listCoTask.clear();
		m_vecDeadObj.clear();
	}

	void CheckMemPool()const noexcept;
};

