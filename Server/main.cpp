#include "pch.h"
#include "IOExecutor.h"
#include "PacketBase.hpp"

int main()
{
	
    if (false == Mgr(IOExecutor)->InitServer("8888"))
    {
        std::cout << "���� �ʱ�ȭ ����" << std::endl;
        return 1;
    }

    Mgr(IOExecutor)->IORoutine(); 
}