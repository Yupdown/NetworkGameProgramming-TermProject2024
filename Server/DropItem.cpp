#include "pch.h"
#include "DropItem.h"
#include "Object.h"
#include "MCWorld.h"
#include "MCTilemap.h"
#include "Session.h"

void DropItem::Update(const float DT)
{
	const auto mc_world = Mgr(MCWorld);
	const auto owner = GetOwner();
	auto& pos_info = owner->GetPosInfo();
	float gravity = 9.8f * 2.0f;
	glm::vec3& pos_pre = pos_info.m_vPos;
	glm::vec3& v = pos_info.m_vVelocity;
	v.y -= gravity * DT;
	glm::vec3 pos_post = pos_pre + v * DT;
	
	bool ground = mc_world->GetTileMap()->HandleCollision(pos_pre, pos_post, v);

	pos_pre = pos_post;

	{
		const auto& players = mc_world->GetWorldObjects(MC_OBJECT_TYPE::PLAYER);
		const glm::ivec3 item_pos = glm::ivec3(pos_pre);
		for (const auto& player : players)
		{
			const auto pos = player->GetPos();
			const glm::ivec3 ipos = glm::ivec3(pos);
			if (ipos == item_pos)
			{
				s2c_ITEM_GET pkt;
				pkt.item_id = m_item_id;
				pkt.item_count = 1;

				auto buffer = player->GetSession()->GetSendBuffer();
				if (buffer != nullptr)
					buffer->Append(pkt);
				owner->SetInvalid();
				return;
			}
		}
	}
}
