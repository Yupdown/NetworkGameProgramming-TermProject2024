#pragma once

template <typename T>
class Singleton
{
private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	Singleton(Singleton&&)noexcept = delete;
	Singleton& operator=(Singleton&&)noexcept = delete;
protected:
	Singleton() = default;
	virtual ~Singleton() = default;
public:
	static T* GetInst()
	{
		static T mgr = {};
		return &mgr;
	}
};