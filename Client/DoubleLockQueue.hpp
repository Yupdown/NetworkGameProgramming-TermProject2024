#pragma once
#include "pch.h"
#include "AtomicMemoryPool.hpp"

#define USE_ATOMIC_ALLOCATER

template <typename T>
class DoubleLockQueue
{
	static const inline HANDLE g_handle = GetProcessHeap();
public:
	struct Node;
	static inline AtomicMemoryPool<Node> g_memPool{ 1024 * 2 };
private:
	struct Node {
		T data;
		Node* next = nullptr;
#ifdef USE_ATOMIC_ALLOCATER
		void* const operator new(const size_t size) noexcept {
			return g_memPool.allocate();
		}

		void operator delete(void* const ptr) noexcept {
			g_memPool.deallocate(ptr);
		}
#else
		void* const operator new(const size_t size) noexcept {
			return ::HeapAlloc(g_handle, NULL, sizeof(Node));
	}

		void operator delete(void* const ptr) noexcept {
			HeapFree(g_handle, NULL, ptr);
		}
#endif
	};
	Node* head;
	SpinLock headLock;
	std::atomic<Node*> tail;
public:
	DoubleLockQueue()noexcept {
		head = tail = new Node;
		head->next = tail.load(std::memory_order_relaxed)->next = nullptr;
	}
	~DoubleLockQueue()noexcept {
		while (head != nullptr) {
			Node* const tmp = head;
			head = head->next;
			std::destroy_at(tmp);
			delete tmp;
		}
	}
	template <typename... Args>
	void emplace(Args&&... args) noexcept {
		Node* const value = new Node;
		std::construct_at(&value->data, std::forward<Args>(args)...);
		Node* oldTail = tail.load(std::memory_order_acquire);
		while (!tail.compare_exchange_weak(oldTail,value
			,std::memory_order_release
			,std::memory_order_relaxed))
		{
		}
		oldTail->next = value;
		std::atomic_thread_fence(std::memory_order_release);
	}
	const bool try_pop(T& _target)noexcept {
		headLock.lock();
		Node* const newHead = head->next;
		if (!newHead)
		{
			headLock.unlock();
			return false;
		}
		Node* const oldHead = head;
		head = newHead;
		if constexpr (std::swappable<T>)
			_target.swap(newHead->data);
		else
			_target = std::move(newHead->data);
		headLock.unlock();
		delete oldHead;
		return true;
	}
	const bool try_pop(T& _target, std::unique_lock<SpinLock>& _lock)noexcept {
		Node* const newHead = head->next;
		if (!newHead)
		{
			headLock.unlock();
			return false;
		}
		Node* const oldHead = head;
		head = newHead;
		if constexpr (std::swappable<T>)
			_target.swap(newHead->data);
		else
			_target = std::move(newHead->data);
		headLock.unlock();
		delete oldHead;
		return true;
	}
	void pop(T& _target, std::unique_lock<SpinLock>& _lock)noexcept {
		Node* const oldHead = head;
		Node* const newHead = head->next;
		_target = std::move(newHead->data);
		head = newHead;
		_lock.unlock();
		delete oldHead;
	}
	const bool empty() noexcept {
		bool bIsEmpty;
		{
			std::lock_guard<SpinLock> lock{ headLock };
			bIsEmpty = !head->next;
		}
		return bIsEmpty;
	}
	void clear() {
		while (!empty()) {
			const Node* const oldHead = head;
			Node* const newHead = head->next;
			newHead->data();
			head = newHead;
			std::cout << oldHead << std::endl;
			delete oldHead;
		}
	}
};


//template <typename T>
//class DoubleLockQueue
//{
//private:
//	struct Node {
//		T data;
//		Node* next;
//	};
//	Node* head;
//	Node* tail;
//	const HANDLE heapHandle = ::GetProcessHeap();
//	SpinLock headLock, tailLock;
//public:
//	DoubleLockQueue()noexcept {
//		head = tail = static_cast<Node* const>(::HeapAlloc(heapHandle, NULL, sizeof(Node)));
//		head->next = tail->next = nullptr;
//	}
//	~DoubleLockQueue()noexcept {
//		while (head != nullptr) {
//			Node* const tmp = head;
//			head = head->next;
//			::HeapFree(heapHandle, NULL, tmp);
//		}
//	}
//	template <typename... Args>
//	void emplace(Args&&... args) noexcept {
//		Node* const value = static_cast<Node* const>(::HeapAlloc(heapHandle, NULL, sizeof(Node)));
//		std::construct_at(&value->data, std::forward<Args>(args)...);
//		value->next = nullptr;
//		std::lock_guard<SpinLock> lock{ tailLock };
//		tail->next = value;
//		tail = value;
//	}
//	const bool try_pop(T& _target)noexcept {
//		//std::lock_guard<SpinLock> lock{ headLock };
//		const Node* const oldHead = head;
//		Node* const newHead = head->next;
//		if (newHead)
//		{
//			_target = std::move(newHead->data);
//			head = newHead;
//			::HeapFree(heapHandle, NULL, oldHead);
//			return true;
//		}
//		else
//		{
//			return false;
//		}
//	}
//	void pop(T& _target, std::unique_lock<SpinLock>& _lock)noexcept {
//		Node* const oldHead = head;
//		Node* const newHead = head->next;
//		_target = std::move(newHead->data);
//		head = newHead;
//		_lock.unlock();
//		::HeapFree(heapHandle, NULL, oldHead);
//	}
//	const bool empty()const noexcept {
//		return !head->next;
//	}
//};