#pragma once

#include <string>
#include <vector>
#include <map>

template <typename T>
class Registry
{
private:
	std::vector<shared_ptr<T>> id_container;
	std::unordered_map<std::string, shared_ptr<T>> key_container;

public:
	Registry();
	~Registry();

	size_t Insert(const std::string& key, T* item);
	size_t Insert(const std::string& key, shared_ptr<T>item);

	T* operator[](int index) const;
	T* operator[](const std::string& key) const;
};

template<typename T>
inline Registry<T>::Registry()
{
}

template<typename T>
inline Registry<T>::~Registry()
{
}

template<typename T>
inline size_t Registry<T>::Insert(const std::string& key, T* item)
{
	const auto& ptr = id_container.emplace_back(item);
	key_container.emplace(key, ptr);

	return id_container.size() - 1;
}

template<typename T>
inline size_t Registry<T>::Insert(const std::string& key, std::shared_ptr<T> item)
{
	id_container.emplace_back(item);
	key_container.emplace(key, std::move(item));

	return id_container.size() - 1;
}

template<typename T>
inline T* Registry<T>::operator[](int index) const
{
	return id_container[index].get();
}

template<typename T>
inline T* Registry<T>::operator[](const std::string& key) const
{
	return key_container.find(key)->second.get();
}
