#include "pch.h"
#include "HP.h"
#include "Object.h"
#include "PacketBase.hpp"
#include "MCWorld.h"

void PlayerHP::AfterDecHPAction(const int hp_)noexcept
{
	s2c_PLAYER_HIT pkt;
	pkt.hit_player_id = GetOwner()->GetObjectID();
	pkt.hit_after_hp = hp_;

	if (IsDead())
	{
		SetHP(PLAYER_START_HP); // 체력리셋
	}

	Mgr(MCWorld)->AppendToWorldSendBuffer(pkt);
}

void EnderManHP::AfterDecHPAction(const int hp_)noexcept
{
	if (IsDead())
	{
		GetOwner()->SetInvalid();
	}
}

void EnderDragonHP::AfterDecHPAction(const int hp_)noexcept
{
}
