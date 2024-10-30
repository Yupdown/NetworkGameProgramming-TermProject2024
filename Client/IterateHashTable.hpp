#pragma once
#include "pch.h"

template<typename T, typename Compare>
constexpr void insertLastInSortedOrder(std::vector<T>& vec,const Compare& cmp)noexcept
{
    const ushort vecSize = static_cast<const ushort>(vec.size());
    if (vecSize < 2) return;

    T lastElement = std::move(vec.back());
    const auto reversed = vec | std::views::reverse | std::views::drop(1);
    const auto it = std::ranges::find_if(reversed, [&lastElement, &cmp](const T& elem) noexcept{
        return !cmp(lastElement, elem);
        });

    const ushort newPos = (const ushort)std::distance(begin(reversed), it);
    std::move_backward(vec.begin() + vecSize - newPos - 1, vec.end() - 1, vec.end());
    vec[vecSize - newPos - 1] = std::move(lastElement);
}

template<typename Key, typename Value>
class IterateHashTable 
{
private:
    std::vector<std::pair<Key, Value>> data;

    auto findPosition(const Key& key) noexcept
    {
        return std::lower_bound(data.begin(),data.end(), key, [](const std::pair<Key, Value>& element, const Key& key)noexcept {
            return element.first < key;
            });
    }

    auto findPosition(const Key& key) const noexcept
    {
        return std::lower_bound(data.begin(), data.end(), key, [](const std::pair<Key, Value>& element, const Key& key)noexcept {
            return element.first < key;
            });
    }

    inline void Sort()noexcept
    {
        //std::ranges::sort(data, [](const std::pair<Key, Value>& a, const std::pair<Key, Value>& b)noexcept {
        //    return a.first < b.first;
        //    });
        insertLastInSortedOrder(data, [](const std::pair<Key, Value>& a, const std::pair<Key, Value>& b)noexcept {
            return a.first < b.first;
            });
    }

public:
    using iterator = typename std::vector<std::pair<Key, Value>>::iterator;
    using const_iterator = typename std::vector<std::pair<Key, Value>>::const_iterator;

    
    void insert(const std::pair<Key, Value>& pair)noexcept
    {
        const auto it = findPosition(pair.first);
        if (it == data.end() || it->first != pair.first)
        {
            data.emplace_back(pair);
            Sort();
        }
    }
  
    template<typename... Args>
    void emplace(Args&&... args)noexcept
    {
        std::pair<Key, Value> pair(std::forward<Args>(args)...);
        const auto it = findPosition(pair.first);
        if (it == data.end() || it->first != pair.first) 
        {
            data.emplace_back(std::move(pair));
            Sort();
        }
    }

    auto erase(const Key& key) noexcept
    {
        auto it = findPosition(key);
        if (it != data.end() && it->first == key)
        {
            return data.erase(it);
        }
        return data.end();
    }

    iterator find(const Key& key) noexcept
    {
        auto it = findPosition(key);
        if (it != data.end() && it->first == key) 
        {
            return it;
        }
        return data.end();
    }

    const_iterator find(const Key& key) const noexcept 
    {
        const auto it = findPosition(key);
        if (it != data.end() && it->first == key) 
        {
            return it;
        }
        return data.end();
    }

    Value& operator[](const Key& key)noexcept
    {
        auto it = findPosition(key);
        if (it != data.end() && it->first == key) 
        {
            return it->second;
        }
        else 
        {
            data.emplace_back(key, Value());
            Sort();
            return findPosition(key)->second;
        }
    }

    const bool contains(const Key& key) const noexcept
    {
        const auto it = findPosition(key);
        return it != data.end() && it->first == key;
    }

    iterator begin() noexcept { return data.begin(); }
    const_iterator begin() const noexcept { return data.begin(); }
    iterator end() noexcept { return data.end(); }
    const_iterator end() const noexcept { return data.end(); }
    size_t size() const noexcept { return data.size(); }
    void reserve(const size_t expandNum)noexcept { data.reserve(expandNum); }
    void clear()noexcept { data.clear(); }
};