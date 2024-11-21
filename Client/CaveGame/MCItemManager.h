#pragma once

#include "Singleton.hpp"
#include "Registry.hpp"
#include "MCItem.h"

class MCItemManager : public Singleton<MCItemManager>
{
private:
	Registry<MCItem> m_registry;

public:
	friend class Singleton;

	MCItemManager();
	~MCItemManager();

	void LoadItems();
	void Initialize();

	MCItem* GetItemByID(int id) const;
	MCItem* GetItemByKey(const std::wstring& key) const;
};

