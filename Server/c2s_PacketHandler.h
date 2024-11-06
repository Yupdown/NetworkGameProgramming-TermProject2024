#pragma once
#include "pch.h"
#include "PacketBase.hpp"

const bool Handle_Invalid(const uint64_t id, const char* const pBuff_);



class c2s_PacketHandler
{
	using PacketHandlerFunc = const bool (*)(const uint64_t, const char* const);
	static inline PacketHandlerFunc g_fpPacketHandler[UINT16_MAX] = {};
public:
	static void Init() noexcept
	{
		for (auto& fpHandlerFunc : g_fpPacketHandler)
		{
			if (nullptr == fpHandlerFunc)
				fpHandlerFunc = Handle_Invalid;
		}
	}

	static const PacketHandlerFunc* const GetPacketHandlerList()noexcept { return g_fpPacketHandler; }

	static void AddProtocol(const uint16_t pktID_, const PacketHandlerFunc fpPacketHandler_)noexcept
	{
		//NAGOX_ASSERT(nullptr == g_fpPacketHandler[pktID_] || Handle_Invalid == g_fpPacketHandler[pktID_]);
		g_fpPacketHandler[pktID_] = fpPacketHandler_;
	}

	static const bool HandlePacket(const uint64_t id, const char* const pBuff_)noexcept
	{
		const PacketHeader* const header = reinterpret_cast<const PacketHeader* const>(pBuff_);
		return g_fpPacketHandler[header->pkt_id](id, pBuff_);
	}
	
public:
	c2s_PacketHandler() = delete;
	c2s_PacketHandler(const c2s_PacketHandler&) = delete;
	c2s_PacketHandler(c2s_PacketHandler&&) = delete;
	c2s_PacketHandler& operator=(const c2s_PacketHandler&) = delete;
	c2s_PacketHandler& operator=(c2s_PacketHandler&&) = delete;
	~c2s_PacketHandler() = delete;
};

void AddProtocol(const uint16_t pktID_, const bool (*fpPacketHandler_)(const uint64_t, const char* const))noexcept;