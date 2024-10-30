#pragma once

template<typename T,unsigned short MAX_SIZE = 1024>
class CircleQueue
{
private:
	T arr[MAX_SIZE] = {};
	unsigned short m_front = 0;
	unsigned short m_rear = 0;
	unsigned short m_count = 0;
public:
	template <typename... Args>
	void emplace(Args&&... args)noexcept {
		arr[m_rear] = T{ std::forward<Args>(args)... };
		++m_count;
		m_rear = ((m_rear + 1) % MAX_SIZE);
	}
	template <typename T>
	void push(T&& _ele)noexcept {
		arr[m_rear] = std::forward<T>(_ele);
		++m_count;
		m_rear = ((m_rear + 1) % MAX_SIZE);
	}
	T& front()noexcept { return arr[m_front]; }
	void pop()noexcept {
		if constexpr (std::is_class_v<T> && !std::same_as<T,function<void(void)>>)arr[m_front].~T();
		--m_count;
		m_front = ((m_front + 1) % MAX_SIZE);
	}
	bool empty()const noexcept { return 0 == m_count; }
};