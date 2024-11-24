#pragma once
#include "pch.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include "SendBufferPool.h"
#include "PacketBase.hpp"

class Session
{
	friend class IOExecutor;
public:
	Session(const uint64_t id, const SOCKET sock)noexcept;
	~Session()noexcept;
	void RegisterSendBuffer()
	{
		if (nullptr == m_cur_send_buff)
			m_cur_send_buff = m_send_buff_pool.GetSendBuffer();
	}
public:
	template<typename T> requires std::derived_from<std::decay_t<T>, PacketHeader>
	void ReserveSend(T&& pkt)noexcept { ReserveSend((char*)&pkt, sizeof(std::decay_t<T>)); }
	const auto GetSessionID()const noexcept { return (uint32_t)m_sessionID; }
	const auto GetSendBuffer()const noexcept { return m_cur_send_buff; }
	RecvBuffer* GetRecvBuffer() { return &m_recvBuff; }
	const auto GetSocket()const noexcept { return m_socket; }
	void SetMyGameObject(S_ptr<class Object> obj)noexcept { m_myGameObject = obj; }
	void ResetSendBuffer() { m_cur_send_buff = nullptr; }
	const auto& GetMyGameObject()const noexcept { return m_myGameObject; }

private:
	void ReturnSendBuffer(SendBuffer* const pBuff)noexcept { m_send_buff_pool.ReturnSendBuffer(pBuff); }
	
	void SetObjectInvalid()noexcept;
	
	void ExecuteSend()noexcept 
	{
		if (const auto len = (ULONG)m_SendBufferForIOThread.GetLen())
		{
			m_wsaBuf.emplace_back(len, m_SendBufferForIOThread.GetBuff());
		}
		if (const auto num = (DWORD)m_wsaBuf.size())
		{
			DWORD sent_byte;
			const int result = ::WSASend(m_socket, m_wsaBuf.data(), num, &sent_byte, NULL, nullptr, nullptr);
			if (SOCKET_ERROR == result)
			{
				std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
			}
			m_SendBufferForIOThread.Clear();
			m_wsaBuf.clear();
			for (const auto buff : m_vecWsaSendBuff)m_send_buff_pool.ReturnSendBuffer(buff);
			m_vecWsaSendBuff.clear();
		}
	}
	void ReserveWSASendForMyBuffer(SendBuffer* const pBuff_)noexcept { 
		m_wsaBuf.emplace_back((ULONG)pBuff_->GetLen(), pBuff_->GetBuff());
		m_vecWsaSendBuff.emplace_back(pBuff_);
	}

	void ReserveWSASend(SendBuffer* const pBuff_) { m_wsaBuf.emplace_back((ULONG)pBuff_->GetLen(), pBuff_->GetBuff()); }

	void ReserveSend(const char* const source, const int pkt_size)noexcept { m_SendBufferForIOThread.Append(source, pkt_size); }
private:
	const uint64_t m_sessionID;
	SOCKET m_socket = INVALID_SOCKET;
	S_ptr<class Object> m_myGameObject;

	RecvBuffer m_recvBuff;

	SendBufferPool m_send_buff_pool;
	SendBuffer* m_cur_send_buff = nullptr;

	SendBuffer m_SendBufferForIOThread;
	
	std::vector<SendBuffer*> m_vecWsaSendBuff;
	std::vector<WSABUF> m_wsaBuf;
};



