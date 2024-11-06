#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

class NetworkManager {
public:
    bool InitClient(const std::string& port);
    void IORoutine();
    void OnRecv(char* buffer);
    void Send(const char* buffer);

private: 
    SOCKET clientSocket = INVALID_SOCKET;
};
