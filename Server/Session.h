#pragma once
#include "pch.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"


class Session
{
public:


private:
	SOCKET m_socket = INVALID_SOCKET;
	RecvBuffer m_recvBuff;

	std::vector<SendBuffer*> m_send_buff_pool;
	std::mutex m_lock;
	SendBuffer* m_cur_send_buff = nullptr;
};

