#pragma once
#include "pch.h"

template <typename T>
class LFQueue
{
public:
	void Push(T* const ptr)noexcept 
	{
		m_arrElements[m_pushIndex].store(ptr);
		m_pushIndex = (m_pushIndex + 1) & (MAX_TASK - 1);
	}
	T* Pop()noexcept
	{
		const auto ptr = m_arrElements[m_popIndex];
		if (ptr)m_popIndex = (m_popIndex + 1) & (MAX_TASK - 1);
		return ptr;
	}
private:
	std::atomic<T*> m_arrElements[MAX_TASK] = { nullptr };
	alignas(64) uint64_t m_popIndex = 0;
	alignas(64) uint64_t m_pushIndex = 0;

	static constexpr uint64_t MAX_TASK = 1024 * 1024;
};