#pragma once
#include "pch.h"

class SendBuffer;

class SendBufferPool
{
public:
	SendBufferPool();
	~SendBufferPool();
	SendBuffer* GetSendBuffer()noexcept;
	void ReturnSendBuffer(SendBuffer* const send_buff)noexcept;
private:
	std::mutex m_lock;
	std::vector<SendBuffer*> m_send_buff_pool;
};
