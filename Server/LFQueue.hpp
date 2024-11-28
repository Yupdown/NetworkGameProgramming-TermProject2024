#pragma once
#include "pch.h"

template <typename T>
class LFQueue
{
	static constexpr uint64_t MAX_TASK = 1024 * 1024;

public:
	void Push(T* const ptr)noexcept 
	{
		m_arrElements[(m_pushIndex++) & (MAX_TASK - 1)].store(ptr);
	}
	template <typename... Args>
	void Push(Args&&... args)noexcept
	{
		Push(new T{ std::forward<Args>(args)... });
	}
	T* Pop()noexcept
	{
		const auto cur_idx = (m_popIndex) & (MAX_TASK - 1);
		const auto ptr = m_arrElements[cur_idx].load();
		if (ptr)
		{
			m_arrElements[cur_idx].store(nullptr,std::memory_order_relaxed);
			++m_popIndex;
		}
		return ptr;
	}
	const bool empty()const noexcept { return m_popIndex == m_pushIndex; }
private:
	alignas(64) std::atomic<T*> m_arrElements[MAX_TASK] = { nullptr };
	alignas(64) volatile uint64_t m_popIndex = 0;
	alignas(64) volatile uint64_t m_pushIndex = 0;

	
};