#include "SendBuffer.h"
#include <string.h>

void SendBuffer::Append(const char* const source, const int pkt_size) noexcept
{
	::memcpy(m_sendBuff + m_len, source, pkt_size);
	m_len += pkt_size;
}
