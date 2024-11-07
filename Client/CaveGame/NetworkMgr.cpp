#include "pch.h"
#include "NetworkMgr.h"
#include "PacketBase.hpp"
#include "s2c_PacketHandler.h"
#include "RecvBuffer.h"

NetworkMgr::NetworkMgr()
    :m_recvBuffer{ std::make_unique<RecvBuffer>() }
{
}

NetworkMgr::~NetworkMgr()
{
    FinishServer();
}

bool NetworkMgr::InitClient(const std::string_view ip, const std::string_view port)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed" << std::endl;
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(std::stoi(port.data()));
    inet_pton(AF_INET, ip.data(), &serverAddr.sin_addr); // 일단 로컬 서버 IP로 설정

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }
  
    u_long mode = 1; 

    if (ioctlsocket(clientSocket, FIONBIO, &mode) != NO_ERROR)
    {
        std::cerr << "Failed to set non-blocking mode: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Successfully connected to server on port :" << port << std::endl;
    return true;
}

void NetworkMgr::IORoutine()
{
    const auto recv_buff = m_recvBuffer.get();

    const int data_len = recv(clientSocket, recv_buff->WritePos(), RecvBuffer::RECV_BUFFER_SIZE::BUFFER_SIZE, 0);

    if (SOCKET_ERROR == data_len)
    {
        const int error = WSAGetLastError();
        if (WSAEWOULDBLOCK != error)
        {
            std::cerr << "Receive error: " << error << std::endl;
        }
        return;
    }

    recv_buff->OnWrite(data_len);

    const int process_len = ::OnRecv(clientSocket, recv_buff->ReadPos(), recv_buff->DataSize(), s2c_PacketHandler::GetPacketHandlerList());

    recv_buff->OnRead(process_len);

    recv_buff->Clear();
}

void NetworkMgr::OnRecv(char* buffer)
{
    std::cout << "Received data: " << buffer << std::endl;
    // 추가적인 처리 
}
void NetworkMgr::FinishServer()
{
    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
}
