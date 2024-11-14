#include "pch.h"
#include "IOExecutor.h"
#include "PacketBase.hpp"
#include "MCWorld.h"

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