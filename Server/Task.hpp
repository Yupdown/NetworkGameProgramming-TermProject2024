#pragma once
#include "pch.h"

class Task
{
public:
	template<typename Func,typename... Args>
	Task(Func&& fp,Args&&... args)noexcept
		:m_fpTask
		{ 
			new std::function<void(void)>{
			[fp = std::forward<Func>(fp),...args = std::forward<Args>(args)]()mutable noexcept
			{std::invoke(std::forward<decltype(fp)>(fp),std::forward<Args>(args)...); }
		}
		}
	{}
public:
	void Execute()const noexcept { m_fpTask->operator()(); delete m_fpTask; }
private:
	std::function<void(void)>* m_fpTask = nullptr;
};