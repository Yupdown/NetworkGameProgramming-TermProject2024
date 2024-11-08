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
public:
    bool InitClient(const std::string_view ip,const std::string_view port);
    void IORoutine();
    void OnRecv(char* buffer);
    template <typename Packet>
    void Send(Packet&& pkt)noexcept
    {
        const int result = send(clientSocket, (char*)&pkt, sizeof(std::decay_t<Packet>), 0);
        if (SOCKET_ERROR == result)[[unlikely]]
        {
            const int error = WSAGetLastError();
            if (WSAEWOULDBLOCK == error)
            {
                std::cerr << "Critical Send Error: " << error << std::endl;
            }
            else
            {
                std::cerr << "Send failed: " << error << std::endl;
            }
        }
    }
    void FinishServer();
private: 
    SOCKET clientSocket = INVALID_SOCKET;
    const std::unique_ptr<RecvBuffer> m_recvBuffer;
};
