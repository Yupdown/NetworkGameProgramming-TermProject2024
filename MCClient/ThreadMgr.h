#pragma once
#include "DoubleLockQueue.hpp"

class ThreadMgr
	:public Singleton<ThreadMgr>
{
	friend class Singleton;
	ThreadMgr();
	~ThreadMgr();
	static constexpr int NUM_OF_THREADS = 6;
private:
	vector<jthread> m_vecThread;
	vector<jthread> m_vecThreadForFuture;
	
	DoubleLockQueue<function<void(void)>> m_jobQ;
	DoubleLockQueue<function<void(void)>> m_futureQ;

	SpinLock m_spinLock = {};
	std::condition_variable_any m_cvQ = {};
	std::atomic<int> m_iCurJobCount = 0;
	
	SpinLock m_spinLockFuture = {};
	std::condition_variable_any m_cvQFuture = {};
	std::atomic<int> m_iFuturejobCount = 0;

	bool	m_bStopRequest = false;

	vector<function<void(void)>> m_vecForMainThreadTask;
	SpinLock m_spinLockForMainThread = {};

	SpinLock m_spinLockForNotify = {};
	std::condition_variable_any m_cvForWakeUp;
public:
	void Init();

	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	void Enqueue(Func&& fp, Args&&... args) noexcept
	{
		m_iCurJobCount.fetch_add(1, std::memory_order_seq_cst);
		m_jobQ.emplace([fp = std::forward<Func>(fp), ...args = std::forward<Args>(args)]()mutable noexcept {std::invoke(std::forward<Func>(fp), std::forward<Args>(args)...); });
		m_cvQ.notify_one();
	}

	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	std::future<std::invoke_result_t<Func, Args...>> EnqueueTaskFuture(Func&& fp, Args&&... args) noexcept
	{
		if ((NUM_OF_THREADS * 2) - 1 <= m_iFuturejobCount.load())
		{
			return std::async(std::launch::async, std::forward<Func>(fp), std::forward<Args>(args)...);
		}
		m_iFuturejobCount.fetch_add(1,std::memory_order_seq_cst);
		using return_type = std::invoke_result_t<Func, Args...>;
		auto task = make_shared<std::packaged_task<return_type(void)>>(std::bind_front(std::forward<Func>(fp), std::forward<Args>(args)...));
		std::future<return_type> res_future = task->get_future();
		m_futureQ.emplace([task = std::move(task)]()mutable noexcept {(*task)(); });
		m_cvQFuture.notify_one();
		return res_future;
	}

	template<typename Func, typename... Args>
		requires std::invocable<Func, Args...>
	void EnqueueTaskForMainThread(Func&& fp, Args&&... args) noexcept
	{
		auto task = [fp = std::forward<Func>(fp), ...args = std::forward<Args>(args)]() mutable noexcept {std::invoke(std::forward<Func>(fp), std::forward<Args>(args)...); };
		{
			std::lock_guard<SpinLock> lock{ m_spinLockForMainThread };
			m_vecForMainThreadTask.emplace_back(std::move(task));
		}
	}

	void ExecuteMainThreadTask()
	{
		for (const auto& task : m_vecForMainThreadTask)
		{
			task();
		}
		m_vecForMainThreadTask.clear();
	}
	
	//void SetJobCount(const int n_) { m_iCurJobCount.store(n_); }

	void WaitAllJob() noexcept
	{
		//while (m_iCurJobCount.load() != numOfJob_) {
		//}
		//m_iCurJobCount.store(0);
		std::unique_lock<SpinLock> lock{ m_spinLockForNotify };
		//m_cvForWakeUp.wait(lock, [this]()noexcept {return 0 == m_iCurJobCount.load(); });

		if (!m_cvForWakeUp.wait_for(lock, std::chrono::seconds(5), [this]() noexcept { return 0 == m_iCurJobCount.load(); }))
		{
			m_jobQ.clear();
			m_futureQ.clear();
			NAGOX_ASSERT(false, "Time Out");
		}
	}
};

