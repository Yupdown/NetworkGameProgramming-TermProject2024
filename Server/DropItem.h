#pragma once
#include "Component.h"

class DropItem
	:public Component
{
public:
	void Update(const float DT)override;
	uint8_t GetItemID() const noexcept { return m_item_id; }
	void SetItemID(const uint8_t id)noexcept { m_item_id = id; }

private:
	uint8_t m_item_id = 0;
};

