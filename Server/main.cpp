#include "pch.h"
#include "IOExecutor.h"
#include "PacketBase.hpp"
#include "MCWorld.h"

int main()
{
	
    if (false == Mgr(IOExecutor)->InitServer("8888"))
    {
        std::cout << "서버 초기화 실패" << std::endl;
        return 1;
    }
    
    Mgr(MCWorld)->Init();


    Mgr(IOExecutor)->IORoutine(); 
}