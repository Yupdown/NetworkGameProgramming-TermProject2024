#pragma once
#include "pch.h"
#include "Singleton.hpp"

class NetworkManager
    :public Singleton<NetworkManager>
{
    friend class Singleton;
    NetworkManager() = default;
    ~NetworkManager() = default;
public:
    bool InitClient(const std::string& port);
    void IORoutine();
    void OnRecv(char* buffer);
    void Send(const char* buffer);

private: 
    SOCKET clientSocket = INVALID_SOCKET;
};
