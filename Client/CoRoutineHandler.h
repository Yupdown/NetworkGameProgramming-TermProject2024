#pragma once
#include "TimeMgr.h"
#include "Component.h"
#include "EventMgr.h"
#include "CoRoutine.hpp"


class CoRoutineHandler
	:public Component,public enable_shared_from_this<CoRoutineHandler>
{
	friend class EventMgr;
	COMP_CLONE(CoRoutineHandler)
private:
	list<CoRoutine> m_listCoTask[etoi(UPDATE_TYPE::END)];
	unordered_map<string,CoRoutine> m_mapCoTask[etoi(UPDATE_TYPE::END)];
public:
	CoRoutineHandler();
	~CoRoutineHandler();
	CoRoutineHandler(const CoRoutineHandler& _other) :Component{COMPONENT_TYPE::COROUTINE_HANDLER} {}
	template<typename Func, typename... Args>
		requires (std::invocable<Func, Args...>&& !std::same_as<CoRoutine, std::invoke_result_t<Func, Args...>>)
	void StartCoRoutine(UPDATE_TYPE _eUpdateType, const float _fInterval,Func&& fp, Args&&... args)
	{
		Mgr(EventMgr)->AddEvent([=, fp = std::forward<Func>(fp), ...args = std::forward<Args>(args)]() mutable {
			this->m_listCoTask[etoi(_eUpdateType)].emplace_back([](const float fInterval,auto Fp,auto arg) mutable ->CoRoutine {
				for (float acc = 0.f; acc < fInterval; acc += DT)
				{
					co_await std::suspend_always{};
				}
				std::apply(std::move(Fp), std::move(arg));
				co_return;
				}(_fInterval,std::forward<Func>(fp),std::make_tuple(std::forward<Args>(args)...))); });
	}
	void StartCoRoutine(UPDATE_TYPE _eUpdateType,CoRoutine&& _coTask) {
		Mgr(EventMgr)->AddEvent([=, tempPtr = make_shared<CoRoutine>(std::move(_coTask))]()mutable {
			this->m_listCoTask[etoi(_eUpdateType)].emplace_back(std::move(*tempPtr)); });
	}
	void StartCoRoutine(UPDATE_TYPE _eUpdateType,const float _fInterval, CoRoutine&& _coTask)
	{
		Mgr(EventMgr)->AddEvent([=, tempPtr = make_shared<CoRoutine>(std::move(_coTask))]()mutable {
			this->m_listCoTask[etoi(UPDATE_TYPE::UPDATE)].emplace_back([](weak_ptr<CoRoutineHandler> coHandle,UPDATE_TYPE eUpdateType,const float fInterval,shared_ptr<CoRoutine> coPtr) mutable ->CoRoutine {
				for (float acc = 0.f; acc < fInterval; acc += DT)
				{
					co_await std::suspend_always{};
				}
				if (const auto ptr = coHandle.lock())
				{
					ptr->StartCoRoutine(eUpdateType, std::move(*coPtr));
				}
				co_return;
				}(this->weak_from_this(), _eUpdateType, _fInterval, std::move(tempPtr))); });
	}

	template<typename Func, typename... Args>
		requires (std::invocable<Func, Args...>&& !std::same_as<CoRoutine, std::invoke_result_t<Func, Args...>>)
	void StartCoRoutineWithKey(UPDATE_TYPE _eUpdateType, string _strName, const float _fInterval,Func&& fp, Args&&... args)
	{
		Mgr(EventMgr)->AddEvent([=,_strName = std::move(_strName), fp = std::forward<Func>(fp), ...args = std::forward<Args>(args)]() mutable {
			this->m_mapCoTask[etoi(_eUpdateType)].emplace(std::move(_strName), [](const float fInterval, auto Fp, auto arg) mutable ->CoRoutine {
				for (float acc = 0.f; acc < fInterval; acc += DT)
				{
					co_await std::suspend_always{};
				}
				std::apply(std::move(Fp), std::move(arg));
				co_return;
				}(_fInterval, std::forward<Func>(fp), std::make_tuple(std::forward<Args>(args)...))); });
	}
	void StartCoRoutineWithKey(UPDATE_TYPE _eUpdateType,string _strName,CoRoutine&& _coTask) {
		Mgr(EventMgr)->AddEvent([=,_strName = std::move(_strName), tempPtr = make_shared<CoRoutine>(std::move(_coTask))]()mutable {
			this->m_mapCoTask[etoi(_eUpdateType)].emplace(std::move(_strName), std::move(*tempPtr)); });
	}
	void StartCoRoutineWithKey(UPDATE_TYPE _eUpdateType,string _strName,const float _fInterval, CoRoutine&& _coTask)
	{
		Mgr(EventMgr)->AddEvent([=,_strName = std::move(_strName), tempPtr = make_shared<CoRoutine>(std::move(_coTask))]() mutable {
			this->m_mapCoTask[etoi(UPDATE_TYPE::UPDATE)].emplace(_strName,[](weak_ptr<CoRoutineHandler> coHandle,string Name, UPDATE_TYPE eUpdateType, const float fInterval,shared_ptr<CoRoutine> coPtr) mutable ->CoRoutine {
				for (float acc = 0.f; acc < fInterval; acc += DT)
				{
					co_await std::suspend_always{};
				}
				if (const auto ptr = coHandle.lock())
				{
					ptr->StartCoRoutineWithKey(eUpdateType,std::move(Name), std::move(*coPtr));
				}
				co_return;
				}(this->weak_from_this(),std::move(_strName), _eUpdateType, _fInterval, std::move(tempPtr))); });
	}
public:
	void Update() override;
	void LateUpdate()override;
	void LastUpdate()override;
	void FinalUpdate() override;

public:
	bool FindCoRoutine(string_view _strKey)const
	{
		static const auto mapContain = [](string_view _strKey ,const unordered_map<string, CoRoutine>& _coMap) {return _coMap.contains(_strKey.data()); };
		return std::ranges::any_of(m_mapCoTask, std::bind_front(mapContain,_strKey));
	}

	bool DestroyCoRoutine(string_view _strKey)
	{
		const bool bRes = FindCoRoutine(_strKey);
		static const auto mapErase = [](string_view _strKey, unordered_map<string, CoRoutine>& _coMap) {_coMap.erase(_strKey.data()); };
		std::ranges::for_each(m_mapCoTask, std::bind_front(mapErase,_strKey));
		return bRes;
	}

	bool IsEmptyCoMap()const { return std::ranges::all_of(m_mapCoTask, std::mem_fn(&unordered_map<string, CoRoutine>::empty)); }

	void ClearCoMap(){std::ranges::for_each(m_mapCoTask, std::mem_fn(&unordered_map<string, CoRoutine>::clear));}

	auto& GetMapCoRoutine() { return m_mapCoTask; }
	
	auto& GetListCoRoutine() { return m_listCoTask; }

	virtual void Save(string_view _resName, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, const fs::path& _savePath) override
	{
		writer.String("CoRoutineHandler");
	}
};

