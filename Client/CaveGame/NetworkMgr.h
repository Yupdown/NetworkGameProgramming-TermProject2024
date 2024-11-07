#pragma once
#include "pch.h"
#include "Singleton.hpp"

class RecvBuffer;

class NetworkMgr
    :public Singleton<NetworkMgr>
{
    friend class Singleton;
    NetworkMgr();
    ~NetworkMgr();
    static constexpr const int SEND_BUFFER = 1024;
public:
    bool InitClient(const std::string_view ip,const std::string_view port);
    void IORoutine();
    void OnRecv(char* buffer);
    template <typename Packet>
    void Send(Packet&& pkt)noexcept
    {
        const int result = send(clientSocket, pkt.SerializeToArray(m_sendBuffer), sizeof(std::decay_t<Packet>), 0);
        if (SOCKET_ERROR == result)
        {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        }
    }
    void FinishServer();
private: 
    SOCKET clientSocket = INVALID_SOCKET;
    const std::unique_ptr<RecvBuffer> m_recvBuffer;
    char m_sendBuffer[SEND_BUFFER];
};
