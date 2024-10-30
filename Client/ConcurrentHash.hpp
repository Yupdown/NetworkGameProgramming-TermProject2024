#pragma once
#include "pch.h"

template <typename Key, typename Value>
class ConcurrentListForMap
{
public:
	struct Node
	{
		std::pair<Key, Value> data;
		Node* next = nullptr;
		template<typename... Args>
		Node(Args&&... args)noexcept :data{ std::forward<Args>(args)... }, next{ nullptr } {}
	};
private:
	Node head;
	mutable SRWLock m_sharedMutex;
	const HANDLE m_handle = GetProcessHeap();
public:
	ConcurrentListForMap() noexcept = default;
	~ConcurrentListForMap() noexcept
	{
		clear();
	}
	ConcurrentListForMap(ConcurrentListForMap&& other) noexcept
		: head(std::move(other.head))
	{
	}
	template<typename ...Args>
	std::pair<Key, Value>* emplace_front(const Key& key_, Args&&... args)noexcept
	{
		Node* const newNode = static_cast<Node* const>(HeapAlloc(m_handle, NULL, sizeof(Node)));
		std::construct_at(&newNode->data, key_, Value{ std::forward<Args>(args)... });
		{
			std::lock_guard<SRWLock> s_lock{ m_sharedMutex };
			newNode->next = head.next;
			head.next = newNode;
		}
		return &newNode->data;
	}
	std::pair<Key, Value>* find(const Key& key_) noexcept
	{
		Node* curNode = &head;
		std::pair<Key, Value>* targetData = nullptr;
		{
			std::shared_lock<SRWLock> s_lock{ m_sharedMutex };
			curNode = head.next;
			while (curNode)
			{
				if (key_ == curNode->data.first)
				{
					targetData = &curNode->data;
					break;
				}
				curNode = curNode->next;
			}
		}
		return targetData;
	}
	const std::pair<Key, Value>* find(const Key& key_)const noexcept
	{
		const Node* curNode = &head;
		const std::pair<Key, Value>* targetData = nullptr;
		{
			std::shared_lock<SRWLock> s_lock{ m_sharedMutex };
			curNode = head.next;
			while (curNode)
			{
				if (key_ == curNode->data.first)
				{
					targetData = &curNode->data;
					break;
				}
				curNode = curNode->next;
			}
		}
		return targetData;
	}
	void erase(const Key& key_)noexcept
	{
		Node* prevNode = &head;
		Node* curNode;
		bool flag = false;
		{
			std::lock_guard<SRWLock> s_lock{ m_sharedMutex };
			curNode = prevNode->next;
			while (curNode)
			{
				if (key_ == curNode->data.first)
				{
					prevNode->next = curNode->next;
					flag = true;
					break;
				}
				prevNode = curNode;
				curNode = curNode->next;
			}
		}
		if (flag)
		{
			::HeapFree(m_handle, NULL, curNode);
		}
	}
	void clear()noexcept
	{
		Node* current = head.next;
		while (current)
		{
			Node* const temp = current;
			current = current->next;
			::HeapFree(m_handle, NULL, temp);
		}
	}
};

template <typename Key, typename Value>
class ConcurrentHashMap
{
private:
	std::vector<ConcurrentListForMap<const Key, Value>> buckets;
	std::hash<Key> hasher;
public:
	ConcurrentHashMap() noexcept :buckets(256) {}

	template <typename ...Args>
	std::pair<const Key, Value>* emplace(const Key& key, Args&&... args) noexcept
	{
		const size_t index = hasher(key) % buckets.size();
		const auto iter = buckets[index].find(key);
		return iter ? iter : buckets[index].emplace_front(key, std::forward<Args>(args)...);
	}

	template <typename ...Args>
	std::pair<std::pair<const Key, Value>*, bool> try_emplace(const Key& key, Args&&... args) noexcept
	{
		const size_t index = hasher(key) % buckets.size();
		const auto iter = buckets[index].find(key);
		return iter ? std::make_pair(iter, false) : std::make_pair(buckets[index].emplace_front(key, std::forward<Args>(args)...), true);
	}

	const std::pair<const Key, Value>* find(const Key& key)const noexcept
	{
		const size_t index = hasher(key) % buckets.size();
		return buckets[index].find(key);
	}

	std::pair<const Key, Value>* find(const Key& key) noexcept
	{
		const size_t index = hasher(key) % buckets.size();
		return buckets[index].find(key);
	}


	void erase(const Key& key) noexcept
	{
		const size_t index = hasher(key) % buckets.size();
		buckets[index].erase(key);
	}

	void reserve(const size_t newCapacity) noexcept
	{
		buckets.resize(newCapacity);
	}

	void clear() noexcept
	{
		for (auto& bucket : buckets)
		{
			bucket.clear();
		}
	}
};

template <typename Value>
class ConcurrentList
{
public:
	struct Node
	{
		Value data;
		Node* next = nullptr;
		template<typename... Args>
		Node(Args&&... args)noexcept :data{ std::forward<Args>(args)... }, next{ nullptr } {}
	};
private:
	Node head;
	mutable SRWLock m_sharedMutex;
	const HANDLE m_handle = GetProcessHeap();
public:
	ConcurrentList() noexcept = default;
	~ConcurrentList() noexcept
	{
		clear();
	}
	ConcurrentList(ConcurrentList&& other) noexcept
		: head(std::move(other.head))
	{
	}
	template<typename ...Args>
	Value* emplace_front(Args&&... args)noexcept
	{
		Node* const newNode = static_cast<Node* const>(HeapAlloc(m_handle, NULL, sizeof(Node)));
		std::construct_at(&newNode->data, std::forward<Args>(args)...);
		{
			std::lock_guard<SRWLock> s_lock{ m_sharedMutex };
			newNode->next = head.next;
			head.next = newNode;
		}
		return &newNode->data;
	}
	Value* find(const Value& val_) noexcept
	{
		Node* curNode = &head;
		Value* targetData = nullptr;
		{
			std::shared_lock<SRWLock> s_lock{ m_sharedMutex };
			curNode = head.next;
			while (curNode)
			{
				if (val_ == curNode->data)
				{
					targetData = &curNode->data;
					break;
				}
				curNode = curNode->next;
			}
		}
		return targetData;
	}
	const Value* find(const Value& val_)const noexcept
	{
		const Node* curNode = &head;
		const Value* targetData = nullptr;
		{
			std::shared_lock<SRWLock> s_lock{ m_sharedMutex };
			curNode = head.next;
			while (curNode)
			{
				if (val_ == curNode->data)
				{
					targetData = &curNode->data;
					break;
				}
				curNode = curNode->next;
			}
		}
		return targetData;
	}
	void erase(const Value& val_)noexcept
	{
		Node* prevNode = &head;
		Node* curNode;
		bool flag = false;
		{
			std::lock_guard<SRWLock> s_lock{ m_sharedMutex };
			curNode = prevNode->next;
			while (curNode)
			{
				if (val_ == curNode->data)
				{
					prevNode->next = curNode->next;
					flag = true;
					break;
				}
				prevNode = curNode;
				curNode = curNode->next;
			}
		}
		if (flag)
		{
			::HeapFree(m_handle, NULL, curNode);
		}
	}
	void clear()noexcept
	{
		Node* current = head.next;
		while (current)
		{
			Node* const temp = current;
			current = current->next;
			::HeapFree(m_handle, NULL, temp);
		}
	}
	vector<Value*> GetAllElements()const noexcept
	{
		vector<Value*> temp;
		Node* curNode = head.next;
		while (curNode)
		{
			temp.emplace_back(&curNode->data);
			curNode = curNode->next;
		}
		return temp;
	}
};

template <typename Key>
class ConcurrentHashSet
{
private:
	std::vector<ConcurrentList<Key>> buckets;
	std::hash<Key> hasher;
public:
	ConcurrentHashSet() noexcept :buckets(256) {}
	~ConcurrentHashSet()noexcept
	{
		clear();
	}
	template <typename ...Args>
	Key* emplace(Args&&... args) noexcept
	{
		Key key{ std::forward<Args>(args)... };
		const size_t index = hasher(key) % buckets.size();
		const auto iter = buckets[index].find(key);
		return iter ? iter : buckets[index].emplace_front(key, std::move(key));
	}

	template <typename ...Args>
	std::pair<Key*, bool> try_emplace(Args&&... args) noexcept
	{
		Key key{ std::forward<Args>(args)... };
		const size_t index = hasher(key) % buckets.size();
		const auto iter = buckets[index].find(key);
		return iter ? std::make_pair(iter, false) : std::make_pair(buckets[index].emplace_front(std::move(key), true));
	}

	const Key* find(const Key& key)const noexcept
	{
		const size_t index = hasher(key) % buckets.size();
		return buckets[index].find(key);
	}

	Key* find(const Key& key) noexcept
	{
		const size_t index = hasher(key) % buckets.size();
		return buckets[index].find(key);
	}

	const bool contains(const Key& key)const noexcept
	{
		return find(key);
	}
	void erase(const Key& key) noexcept
	{
		const size_t index = hasher(key) % buckets.size();
		buckets[index].erase(key);
	}

	void reserve(const size_t newCapacity) noexcept
	{
		buckets.resize(newCapacity);
	}

	void clear() noexcept
	{
		for (auto& bucket : buckets)
		{
			bucket.clear();
		}
	}
	vector<Key*> GetAllElements()noexcept
	{
		vector<Key*> temp;
		temp.reserve(100);
		for (const auto& bucket : buckets)
		{
			for (const auto& ele : bucket.GetAllElements())
			{
				temp.emplace_back(ele);
			}
		}
		return temp;
	}
};