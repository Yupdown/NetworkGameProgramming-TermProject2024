#pragma once
#include "pch.h"
#include "TimeMgr.h"
#include "ThreadMgr.h"

class WaitForSeconds;

class CoRoutine
{
public:
	struct promise_type;
	using handle = std::coroutine_handle<promise_type>;
	struct  promise_type
	{
		WaitForSeconds* pAwaitable = nullptr;
		std::any yield_return_value = {};
		CoRoutine get_return_object() { return CoRoutine{ handle::from_promise(*this) }; }
		auto initial_suspend() { return std::suspend_always{}; }
		auto final_suspend()noexcept { return std::suspend_always{}; }
		auto yield_value(std::any _yieldVal) {
			yield_return_value = std::move(_yieldVal);
			return std::suspend_always{};
		}
		void return_void() {}
		void unhandled_exception() { assert(false); }
	};
	handle m_coHandle = nullptr;
	std::future<bool> m_coroFuture;
	CoRoutine() = default;
	~CoRoutine()
	{
		if (nullptr != m_coHandle)
			m_coHandle.destroy();
		m_coHandle = nullptr;
	}
	CoRoutine(const CoRoutine&) = delete;
	CoRoutine& operator=(const CoRoutine&) = delete;
	CoRoutine(CoRoutine&& _other) noexcept
	{
		if (nullptr != m_coHandle)
			m_coHandle.destroy();
		m_coHandle = _other.m_coHandle;
		_other.m_coHandle = nullptr;
	}
	CoRoutine& operator = (CoRoutine&& _other)noexcept
	{
		if (nullptr != m_coHandle)
			m_coHandle.destroy();
		m_coHandle = _other.m_coHandle;
		_other.m_coHandle = nullptr;
		return *this;
	}
	CoRoutine(handle _h) :m_coHandle{ _h } {}
	operator bool()
	{
		if (nullptr != m_coHandle)
			return true;
		else
			return false;
	}
	bool done()const { return m_coHandle.done(); }
private:
	void reset()
	{
		if (nullptr != m_coHandle)
			m_coHandle.destroy();
		m_coHandle = nullptr;
	}
public:
	bool resume()
	{
		if (nullptr != m_coHandle && !m_coHandle.done())
		{
			if (WaitSecondsHelper(m_coHandle.promise().pAwaitable))
			{
				m_coHandle.resume();
			}

			return true;
		}
		else
		{
			reset();
			return false;
		}
	}
	template <typename T>
	std::optional<T> getCurrentAsync() {
		std::optional<T> future_yield_value;
		if (!m_coroFuture.valid())
		{
			future_yield_value = resume() ? std::make_optional(std::any_cast<T&&>(std::move(m_coHandle.promise().yield_return_value))) : std::nullopt;
		}
		else
		{
			if (!m_coroFuture.get() || done())
			{
				return std::nullopt;
			}
			else
			{
				future_yield_value = std::make_optional(std::any_cast<T&&>(std::move(m_coHandle.promise().yield_return_value)));
			}
		}
		m_coroFuture = Mgr(ThreadMgr)->EnqueueTaskFuture(&CoRoutine::resume, this);
		return future_yield_value;
	}
	template <typename T>
	std::optional<T> getCurrent() {
		return resume() ? std::make_optional(std::any_cast<T&&>(std::move(m_coHandle.promise().yield_return_value))) : std::nullopt;
	}
};

class WaitForSeconds
{
	friend bool WaitSecondsHelper(WaitForSeconds*& _pWait)noexcept;
private:
	const float m_waitTime;
	float m_accTime = 0.f;
public:
	explicit WaitForSeconds(const float _waitTime):m_waitTime{_waitTime},m_accTime{ DT }{}
	~WaitForSeconds() = default;
	bool await_ready() noexcept { return false; }
	void await_suspend(CoRoutine::handle _handle) noexcept {_handle.promise().pAwaitable = this;}
	void await_resume() const noexcept {}
};