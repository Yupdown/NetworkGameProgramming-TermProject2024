#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

class MCItemTable
{
public:
	MCItemTable() = delete;
	~MCItemTable() = delete;
public:
	static void Init();

	template<typename T>
	static auto CreateItemForClient(const std::string_view key)noexcept {
		const auto& p = g_strTable[key.data()];
		return std::make_shared<T>(p.first, p.second);
	}
public:
	static const auto& GetStrTable() noexcept { return g_strTable; }

	static const auto StrToIdx(const std::string_view name)noexcept { return g_strToIdx.find(name.data())->second; }

	static const auto& IdxToStr(const int idx)noexcept { return g_idxToString[idx]; }
private:
	static void RegisterTable(const std::string_view a, const std::string_view b, const std::string_view c) {
		g_strTable.try_emplace(a.data(), b.data(), c.data());
		const auto& target = g_idxToString.emplace_back(a.data());
		g_strToIdx.try_emplace(a.data(), (int)(&target - g_idxToString.data()));
	}
private:
	static inline std::unordered_map<std::string, std::pair<std::string, std::string>> g_strTable;
	static inline std::unordered_map<std::string, int> g_strToIdx;
	static inline std::vector<std::string> g_idxToString;
};

