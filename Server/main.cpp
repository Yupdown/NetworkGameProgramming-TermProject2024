#include "pch.h"
#include "IOExecutor.h"
#include "PacketBase.hpp"
#include "MCWorld.h"
#include "MCItemTable.h"

int main()
{
    if (false == Mgr(IOExecutor)->InitServer("8888"))
    {
        std::cout << "서버 초기화 실패" << std::endl;
        return 1;
    }
    MCItemTable::Init();
    Mgr(MCWorld)->Init();

    std::atomic_thread_fence(std::memory_order_seq_cst);

    std::thread io_thread{ []() {Mgr(IOExecutor)->IORoutine(); } };

    std::string exit_word;
    for (;;)
    {
        std::cin >> exit_word;
        if ("EXIT" == exit_word)
        {
            Mgr(IOExecutor)->EndServer();
            break;
        }
    }
    io_thread.join();
}