#pragma once
#include "pch.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"


class Session
{
public:
	Session(const uint64_t id, const SOCKET sock) 
		: m_sessionID{ id }
		, m_socket{ sock } 
	{}
public:
	const auto GetSocket()const noexcept { return m_socket; }
	const auto GetSendBuffer()const noexcept { return m_cur_send_buff; }
	void ReturnSendBuffer(SendBuffer* const pBuff)noexcept {
		std::scoped_lock lk{ m_lock };
		m_send_buff_pool.emplace_back(pBuff);
	}
	RecvBuffer* GetRecvBuffer() { return &m_recvBuff; }
	const auto GetSessionID()const noexcept { return m_sessionID; }
public:
	void RegisterSendBuffer() {
		std::scoped_lock lk{ m_lock };
		if (m_send_buff_pool.empty())m_cur_send_buff = new SendBuffer;
		else {
			m_cur_send_buff = m_send_buff_pool.back();
			m_send_buff_pool.pop_back();
		}
	}
public:

private:
	const uint64_t m_sessionID;
	SOCKET m_socket = INVALID_SOCKET;
	RecvBuffer m_recvBuff;

	std::vector<SendBuffer*> m_send_buff_pool;
	std::mutex m_lock;
	SendBuffer* m_cur_send_buff = nullptr;
};

