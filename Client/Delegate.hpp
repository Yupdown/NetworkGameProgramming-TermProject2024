#pragma once
#include "pch.h"


class Delegate
{
private:
	vector<function<void(void)>> vecFp;
public:
	void operator += (function<void(void)> _fp) { vecFp.emplace_back(std::move(_fp)); }
	void operator()() const { std::ranges::for_each(vecFp,std::mem_fn(&function<void(void)>::operator())); }
	operator bool() const { return !vecFp.empty(); }
	void clear() { vecFp.clear(); }
};
