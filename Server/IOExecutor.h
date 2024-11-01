#pragma once
#include "Singleton.hpp"

class IOExecutor
	:public Singleton<IOExecutor>
{
	friend class Singleton;
	IOExecutor() = default;
	~IOExecutor() = default;

	static constexpr int NUM_OF_CLIENTS = 10;
public:
	bool InitServer(std::string_view port);
	void IORoutine()noexcept;
private:
	SOCKET m_serverSocket = INVALID_SOCKET;
	WSAPOLLFD m_clientsFD[NUM_OF_CLIENTS + 1];
};

