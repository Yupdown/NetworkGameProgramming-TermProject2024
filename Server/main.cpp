#include "pch.h"
#include "IOExecutor.h"
#include "PacketBase.hpp"
#include "MCWorld.h"

#include <concepts>
#include <type_traits>
#include <cstdint>
#include <windows.h>
#include <concepts>
#include <type_traits>
#include <cstdint>
#include <windows.h>

int main()
{
    if (false == Mgr(IOExecutor)->InitServer("8888"))
    {
        std::cout << "���� �ʱ�ȭ ����" << std::endl;
        return 1;
    }
    
    Mgr(MCWorld)->Init();


    Mgr(IOExecutor)->IORoutine(); 
}