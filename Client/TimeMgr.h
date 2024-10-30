#pragma once

class TimeMgr
	:public Singleton<TimeMgr>
{
	friend class Singleton;
	TimeMgr();
	~TimeMgr();
private:
	std::chrono::steady_clock::time_point m_InitTime;
	std::chrono::steady_clock::time_point m_PrevTime;
	std::chrono::duration<float> m_DeltaTime;
	unordered_map<string, function<void(void)>> m_mapTimer;
	float m_accFPS = 0.f;
public:
	void Init();
	void Update()noexcept;
	const float GetDT()const noexcept{ return std::min(0.016f, m_DeltaTime.count()); }
	const auto GetGameTime()const noexcept { return std::chrono::steady_clock::now() - m_InitTime; }
	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	void AddTimer(string_view _strName,const float _fInterval, Func&& fp, Args&&... args)
	{
		m_mapTimer.emplace(_strName, [=, fp = std::forward<Func>(fp), ...args = std::forward<Args>(args)]()mutable {
			static float AccTime = 0.f;
			AccTime += DT;
			if (_fInterval <= AccTime)
			{
				std::invoke(std::forward<Func>(fp), std::forward<Args>(args)...);
				AccTime = 0.f;
			}
			});
	}
	void KillTimer(string_view _strName) { m_mapTimer.erase(_strName.data()); }
};

