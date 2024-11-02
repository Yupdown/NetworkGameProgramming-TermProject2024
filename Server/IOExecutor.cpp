#include "pch.h"
#include "IOExecutor.h"

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
                // TODO: accept + 월드 입장 + 브로드캐스트
            }

            for (int i = 1; i <= NUM_OF_CLIENTS; ++i)
            {
                if (m_clientsFD[i].revents & POLLRDNORM)
                {
                    // TODO: Recv루틴
                }
                else if (m_clientsFD[i].revents & POLLHUP)
                {
                    // TODO: Disconnect, 퇴장루틴
                }
            }
        }
    }
        // TODO: SendQueue Flush
}
