#include "pch.h"
#include "ThreadMgr.h"

ThreadMgr::ThreadMgr()
{
}

ThreadMgr::~ThreadMgr()
{
	m_bStopRequest = true;

	m_cvQ.notify_all();
	m_cvQFuture.notify_all();

	for (int i = 0; i < NUM_OF_THREADS; ++i)
	{
		m_vecThread[i].join();
		m_vecThreadForFuture[i].join();
	}
}

void ThreadMgr::Init()
{
	for (size_t i = 0; i < NUM_OF_THREADS; ++i)
	{
		m_vecThread.emplace_back([this] {
			while (true)
			{
				std::function<void(void)> task;
				{
					std::unique_lock<SpinLock> lock{ m_spinLock };
					//m_cvQ.wait(lock, [this] () noexcept {
					//	return not m_jobQ.empty() || m_bStopRequest;
					//	});
					m_cvQ.wait(lock, [this,&task,&lock]() noexcept {
						return m_jobQ.try_pop(task,lock) || m_bStopRequest;
						});
					//m_cvQ.wait(lock, [this,&task]() noexcept {
					//	return m_jobQ.try_pop(task) || m_bStopRequest;
					//	});
					if (m_bStopRequest)
					{
						return;
					}
					//m_jobQ.pop(task,lock);
				}

				task();

				//std::atomic_thread_fence(std::memory_order_seq_cst);

				//m_iCurJobCount.fetch_add(1, std::memory_order_seq_cst);
				if (0 == --m_iCurJobCount)
				{
					std::lock_guard<SpinLock> notifyLock{ m_spinLockForNotify };
					m_cvForWakeUp.notify_one();
				}
			}
			});

		m_vecThreadForFuture.emplace_back([this] {
			while (true)
			{
				std::function<void(void)> task;
				{
					std::unique_lock<SpinLock> lock{ m_spinLockFuture };
					//m_cvQFuture.wait(lock, [this]() noexcept {
					//	return not m_futureQ.empty() || m_bStopRequest;
					//	});
					m_cvQFuture.wait(lock, [this,&task]() noexcept {
						return m_futureQ.try_pop(task) || m_bStopRequest;
						});
					//m_cvQFuture.wait(lock, [this,&task]() noexcept {
					//	return m_futureQ.try_pop(task) || m_bStopRequest;
					//	});
					if (m_bStopRequest)
					{
						return;
					}
					//m_futureQ.pop(task, lock);
				}

				task();

				//std::atomic_thread_fence(std::memory_order_seq_cst);

				m_iFuturejobCount.fetch_sub(1, std::memory_order_seq_cst);
			}
			});
	}
}
