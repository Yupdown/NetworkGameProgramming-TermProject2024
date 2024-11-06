#include "pch.h"
#include "s2c_PacketHandler.h"

const bool Handle_Invalid(const uint64_t id, const char* const pBuff_)
{
	return false;
}

void AddProtocol(const uint16_t pktID_, const bool(*fpPacketHandler_)(const uint64_t, const char* const)) noexcept
{
	s2c_PacketHandler::AddProtocol(pktID_, fpPacketHandler_);
}
