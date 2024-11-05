#include "pch.h"
#include "IOExecutor.h"
#include "PacketBase.hpp"

int main()
{
	c2s_TEST t;

	char* p[100];
	t.SerializeToArray(p);
	// if (false == Mgr(IOExecutor)->InitServer("8888"))
	// {
	// 	std::cout << "서버 초기화 실패" << std::endl;
	// 	return 1;
	// }
}