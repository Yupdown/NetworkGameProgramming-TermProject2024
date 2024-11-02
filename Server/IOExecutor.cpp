#include "pch.h"
#include "IOExecutor.h"

bool IOExecutor::InitServer(std::string_view port)
{
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        std::cerr << "WSAStartup ����" << std::endl;
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
        std::cerr << "���� ���� ����" << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(std::stoi(port.data()));
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) 
    {
        std::cerr << "���ε� ����" << std::endl;
        closesocket(m_serverSocket);
        WSACleanup();
        return false;
    }

    if (listen(m_serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "���� ��� ����" << std::endl;
        closesocket(m_serverSocket);
        WSACleanup();
        return false;
    }

    m_clientsFD[0].fd = m_serverSocket;
    m_clientsFD[0].events = POLLRDNORM;

    std::cout << "������ ��Ʈ " << port << "���� ���� ���Դϴ�." << std::endl;

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
            std::cerr << "WSAPoll ����: " << WSAGetLastError() << std::endl;
            break;
        }

        if (1 <= pollResult)
        {
            // ���� ��û ���� �� ������ ó��
            if (m_clientsFD[0].revents & POLLRDNORM)
            {
                // TODO: accept + ���� ���� + ��ε�ĳ��Ʈ
            }

            for (int i = 1; i <= NUM_OF_CLIENTS; ++i)
            {
                if (m_clientsFD[i].revents & POLLRDNORM)
                {
                    // TODO: Recv��ƾ
                }
                else if (m_clientsFD[i].revents & POLLHUP)
                {
                    // TODO: Disconnect, �����ƾ
                }
            }
        }
    }
        // TODO: SendQueue Flush
}
