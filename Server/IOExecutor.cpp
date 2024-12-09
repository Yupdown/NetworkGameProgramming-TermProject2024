#include "pch.h"
#include "IOExecutor.h"
#include "Session.h"
#include "PacketBase.hpp"
#include "c2s_PacketHandler.h"
#include "MCWorld.h"

bool IOExecutor::InitServer(std::string_view port)
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        std::cerr << "WSAStartup 실패" << std::endl;
        return false;
    }

    for (int i = 0; i < NUM_OF_CLIENTS + 1; ++i)
    {
        m_clientsFD[i].fd = INVALID_SOCKET;
        m_clientsFD[i].events = 0;
        m_clientsFD[i].revents = 0;
    }

    m_serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_serverSocket == INVALID_SOCKET) 
    {
        std::cerr << "소켓 생성 실패" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(std::stoi(port.data()));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) 
    {
        std::cerr << "바인딩 실패" << std::endl;
        closesocket(m_serverSocket);
        WSACleanup();
        return false;
    }

    if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "수신 대기 실패" << std::endl;
        closesocket(m_serverSocket);
        WSACleanup();
        return false;
    }

    m_clientsFD[0].fd = m_serverSocket;
    m_clientsFD[0].events = POLLRDNORM;

    std::cout << "서버가 포트 " << port << "에서 실행 중입니다." << std::endl;

    m_bIsRunning = true;

    return true;
}

void IOExecutor::IORoutine() noexcept
{
    while (m_bIsRunning)
    {
        const int pollResult = WSAPoll(m_clientsFD, NUM_OF_CLIENTS + 1, 0);

        if (SOCKET_ERROR == pollResult)
        {
            std::cerr << "WSAPoll 실패: " << WSAGetLastError() << std::endl;
            break;
        }
        
        if (1 <= pollResult)
        {
            // 연결 요청 수락 및 데이터 처리
            if (m_clientsFD[0].revents & POLLRDNORM)
            {
                OnAccept();
            }

            for (int i = 1; i <= NUM_OF_CLIENTS;)
            {
                const auto sock = m_clientsFD[i].fd;
                if (INVALID_SOCKET == sock)break;
                if (m_clientsFD[i].revents & POLLRDNORM)
                {
                    OnRecv(sock);
                }
                else if (m_clientsFD[i].revents & POLLHUP)
                {
                    OnDisconnect(sock, i);
                    continue;
                }
                ++i;
            }
        }
        FlushSendQueue();
    }
}

void IOExecutor::OnAccept() noexcept
{
    if (NUM_OF_CLIENTS <= m_curNumOfClient)return;

    const int cur_idx = ++m_curNumOfClient;

    SOCKADDR_STORAGE sockAddr;
    int nAddrLen = sizeof(SOCKADDR_STORAGE);
    m_clientsFD[cur_idx].fd = accept(m_serverSocket, (LPSOCKADDR)&sockAddr, &nAddrLen);
    m_clientsFD[cur_idx].events = POLLRDNORM;

    const auto cur_ID = GetObjectIDAndIncrement();
    auto session = std::make_shared<Session>(cur_ID, m_clientsFD[cur_idx].fd);

    BOOL flag = TRUE;

    if (setsockopt(m_clientsFD[cur_idx].fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(flag)) != 0)
    {
        std::cerr << "Failed to disable Nagle's algorithm: " << WSAGetLastError() << std::endl;
    }

    u_long mode = 1;

    if (ioctlsocket(m_clientsFD[cur_idx].fd, FIONBIO, &mode) != NO_ERROR)
    {
        std::cerr << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
    }

    m_mapSession.try_emplace(cur_ID, session);
    m_mapSocket2Session.try_emplace(m_clientsFD[cur_idx].fd, std::move(session));

    std::cout << "Client In\n";
}

void IOExecutor::OnDisconnect(const SOCKET sock ,const int idx) noexcept
{
    const auto iter = m_mapSocket2Session.find(sock);
    if (m_mapSocket2Session.cend() == iter)return;
    const auto& session = iter->second;
    session->SetObjectInvalid();

    const auto session_id = session->GetSessionID();

    const auto last_idx = m_curNumOfClient--;
    std::swap(m_clientsFD[idx], m_clientsFD[last_idx]);

    m_clientsFD[last_idx].fd = INVALID_SOCKET;
    m_clientsFD[last_idx].events = 0;
    m_clientsFD[last_idx].revents = 0;

    m_mapSocket2Session.erase(iter);
    m_mapSession.erase(session_id);

    s2c_REMOVE_OBJECT pkt;
    pkt.object_id = session_id;

    AppendToSendBuffer(pkt);

    std::cout << "Client Out\n";
}

void IOExecutor::OnRecv(const SOCKET sock) noexcept
{
    const auto iter = m_mapSocket2Session.find(sock);
    if (m_mapSocket2Session.cend() == iter)return;
    const auto session = iter->second;

    const auto recv_buff = session->GetRecvBuffer();

    const int data_len = recv(sock, recv_buff->WritePos(), recv_buff->FreeSize(), 0);
    recv_buff->OnWrite(data_len);

    const int process_len = ::OnRecv(session->GetSessionID(),recv_buff->ReadPos(), recv_buff->DataSize(), c2s_PacketHandler::GetPacketHandlerList());

    recv_buff->OnRead(process_len);

    recv_buff->Clear();
}

void IOExecutor::FlushSendQueue() noexcept
{
    const auto mc_world = Mgr(MCWorld);

    int limit_len = 0;
    while (const auto broad_event = m_broadCastQueue.Pop()) {
        limit_len += m_flush_buffer.emplace_back(broad_event)->GetLen();
        if (8192 <= limit_len)break;   
    }

    const auto sentinel = m_mapSession.cend();

    while (const auto send_event = m_sendQueue.Pop())
    {
        const auto iter = m_mapSession.find(send_event->id);
        const auto send_buff = send_event->send_buff;
        if (iter != sentinel)
        {
            const auto& session = iter->second;
            session->ReserveWSASendForMyBuffer(send_buff);
        }
        else
        {
            delete send_buff;
        }
        delete send_event;
    }

    const auto io_len = m_sendBuff.GetLen();
    const bool flag = 0 != io_len;
    {
        const auto sentinel = m_mapSocket2Session.cend();

        for (int i = 1; i <= m_curNumOfClient; ++i)
        {
            const auto iter = m_mapSocket2Session.find(m_clientsFD[i].fd);
            if (sentinel == iter)continue;
            const auto& session = iter->second;
            if (flag)
                session->ReserveWSASend(&m_sendBuff);
            for (const auto broad_cast_buff : m_flush_buffer)
            {
                session->ReserveWSASend(broad_cast_buff);
            }
            session->ExecuteSend();
        }
    }

    m_sendBuff.Clear();
   

    for (const auto broad_cast_buff : m_flush_buffer)
    {
        mc_world->ReturnSendBufferToWorld(broad_cast_buff);
    }

    m_flush_buffer.clear();
}
