#pragma once
#include "Constants.h"

class SendBuffer
{
	static constexpr int SEND_BUFF_SIZE = 1024 * 8;
public:
	void Append(const char* const source, const int pkt_size)noexcept;

	const char* const GetBuff()const noexcept { return m_sendBuff; }
	int GetLen()const noexcept { return m_len; }
	void Clear()noexcept { m_len = 0; }
private:
	char m_sendBuff[SEND_BUFF_SIZE];
	int m_len = 0;
};