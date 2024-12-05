#include "pch.h"
#include "HP.h"
#include "Object.h"
#include "PacketBase.hpp"
#include "MCWorld.h"

void PlayerHP::AfterDecHPAction(const int hp_)noexcept
{
	if (IsDead())
	{
		SetHP(PLAYER_START_HP); // 체력리셋
	}

	SendHitPacket(GetOwner(), hp_);
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
	std::cout << "Boss Hit ! \n";
	const auto owner = GetOwner();
	if (IsDead())
	{
		owner->SetInvalid();
	}
	SendHitPacket(owner, hp_);
}

void HP::SendHitPacket(const Object* const owner, const int hp_)noexcept
{
	s2c_OBJECT_HIT pkt;
	pkt.hit_object_id = owner->GetObjectID();
	pkt.hit_after_hp = hp_;
	Mgr(MCWorld)->AppendToWorldSendBuffer(pkt);
}
