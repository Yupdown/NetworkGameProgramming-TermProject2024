#include "pch.h"
#include "MCItem.h"
#include "PacketBase.hpp"
#include "NetworkMgr.h"

MCItem::MCItem(std::string name, std::string k_element)
{
	m_name = name;
	m_iconTexture = k_element;
	m_maxStackSize = 99;
}

MCItem::~MCItem()
{
}

std::string MCItem::GetName() const
{
	return m_name;
}

int MCItem::GetMaxStacksize() const
{
	return m_maxStackSize;
}

std::string MCItem::GetIconTexture() const
{
	return m_iconTexture;
}

bool MCItem::OnUseItem(MCTilemap* world, Hero* owner, const RaycastResult& raycast)
{
	return false;
}

MCItemBlock::MCItemBlock(std::string name, std::string k_element, uint8_t tileID)
	:MCItem(name, k_element), m_tileID(tileID)
{
}

bool MCItemBlock::OnUseItem(MCTilemap* world, Hero* owner, const RaycastResult& raycast)
{
	if (!raycast.hit)
		return false;

	glm::ivec3 placePos = raycast.hitTilePosition + glm::ivec3(raycast.hitNormal);

	// 블럭을 설치하려는 위치에 플레이어가 서 있으면 블럭을 설치하지 않는다.
	glm::ivec3 playerPos = glm::ivec3(owner->GetPosition());
	glm::ivec3 deltaPos = placePos - playerPos;
	if (deltaPos != glm::ivec3(0, 0, 0) && deltaPos != glm::ivec3(0, 1, 0))
	{
		// TODO: 수정한부분
		const auto val = raycast.hitTilePosition + glm::ivec3(raycast.hitNormal);
		c2s_CREATE_BLOCK pkt;
		pkt.x = val.x;
		pkt.y = val.y;
		pkt.z = val.z;
		pkt.tile_id = m_tileID;
		Mgr(NetworkMgr)->Send(pkt);
	}

	return true;
}

MCItemCallback::MCItemCallback(std::string name, std::string k_element, std::function<void(MCTilemap*, Hero*, const RaycastResult&)> callback)
	:MCItem(name, k_element), m_callback(callback)
{

}

bool MCItemCallback::OnUseItem(MCTilemap* world, Hero* owner, const RaycastResult& raycast)
{
	if (m_callback)
	{
		m_callback(world, owner, raycast);
		return true;
	}
	return false;
}