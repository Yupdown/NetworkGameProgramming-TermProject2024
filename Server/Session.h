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
	~Session() { ::closesocket(m_socket); }
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
	//template <typename Packet>
	//void SendDirect(Packet&& pkt)
	//{
	//	// TODO: 이 함수는 없어지고 Append 후 센드큐에서 일괄처리 할 수도 있음
	//	const int result = send(m_socket, (char*)&pkt, sizeof(std::decay_t<Packet>), 0);
	//	if (SOCKET_ERROR == result)
	//	{
	//		std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
	//	}
	//}
	void ExecuteSend()noexcept 
	{
		//	// TODO: 이 함수는 없어지고 Append 후 센드큐에서 일괄처리 할 수도 있음
		if (const auto len = m_SendBufferForIOThread.GetLen())
		{
			const int result = send(m_socket, m_SendBufferForIOThread.GetBuff(), len, 0);
			if (SOCKET_ERROR == result)
			{
				std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
			}
			m_SendBufferForIOThread.Clear();
		}
	}
	void ReserveSend(const char* const source, const int pkt_size)noexcept { m_SendBufferForIOThread.Append(source, pkt_size); }
	template<typename T>
	void ReserveSend(T&& pkt)noexcept { ReserveSend((char*)&pkt, sizeof(std::decay_t<T>)); }
private:
	const uint64_t m_sessionID;
	SOCKET m_socket = INVALID_SOCKET;
	RecvBuffer m_recvBuff;

	std::vector<SendBuffer*> m_send_buff_pool;
	std::mutex m_lock;
	SendBuffer* m_cur_send_buff = nullptr;

	SendBuffer m_SendBufferForIOThread;
};

