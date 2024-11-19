#include "pch.h"
#include "SendBufferPool.h"
#include "SendBuffer.h"

SendBufferPool::SendBufferPool()
{
	constexpr int NUM_OF_BUFF = 10;
	m_send_buff_pool.reserve(NUM_OF_BUFF);
	for (int i = 0; i < NUM_OF_BUFF; ++i)
		m_send_buff_pool.emplace_back(new SendBuffer);
}

SendBufferPool::~SendBufferPool()
{
	for (const auto buff : m_send_buff_pool)delete buff;
}

SendBuffer* SendBufferPool::GetSendBuffer() noexcept
{
	std::scoped_lock lk{ m_lock };
	if (m_send_buff_pool.empty())
	{
		return new SendBuffer;
	}
	else 
	{
		const auto temp = m_send_buff_pool.back();
		m_send_buff_pool.pop_back();
		return temp;
	}
}

void SendBufferPool::ReturnSendBuffer(SendBuffer* const send_buff) noexcept
{
	std::scoped_lock lk{ m_lock };
	m_send_buff_pool.emplace_back(send_buff);
}
