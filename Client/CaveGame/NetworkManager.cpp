#include "pch.h"
#include "NetworkManager.h"

bool NetworkManager::InitClient(const std::string& port)
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
    serverAddr.sin_port = htons(std::stoi(port));
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // 일단 로컬 서버 IP로 설정

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return false;
    }

    std::cout << "Successfully connected to server on port :" << port << std::endl;
    return true;
}

void NetworkManager::IORoutine() {
    char buffer[512];
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead > 0) {
        OnRecv(buffer);
    }
    else if (bytesRead == 0) {
        std::cout << "Connection closed" << std::endl;
    }
    else {
        std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
    }
}

void NetworkManager::OnRecv(char* buffer)
{
    std::cout << "Received data: " << buffer << std::endl;
    // 추가적인 처리 
}

void NetworkManager::Send(const char* buffer) {
    int result = send(clientSocket, buffer, static_cast<int>(strlen(buffer)), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
    }
}
