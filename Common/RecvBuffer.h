#pragma once
#include "Constants.h"

class RecvBuffer
{
	enum { BUFFER_COUNT = 8 };
public:
	RecvBuffer(const int bufferSize_ = RecvBuffer::BUFFER_SIZE)noexcept;
	~RecvBuffer();
public:
	enum RECV_BUFFER_SIZE
	{
		BUFFER_SIZE = 0x2000 - (40 / BUFFER_COUNT), // 8KB
	};

	void Clear()noexcept;
	bool OnRead(const int numOfBytes)noexcept
	{
		if (numOfBytes > DataSize()) [[unlikely]]
			return false;

			m_readPos += numOfBytes;

			return true;
	}
	bool OnWrite(const int numOfBytes)noexcept
	{
		if (numOfBytes > FreeSize()) [[unlikely]]
			return false;

			m_writePos += numOfBytes;

			return true;
	}

	char* const ReadPos() noexcept { return m_buffer + m_readPos; }
	char* const WritePos() noexcept { return m_buffer + m_writePos; }
	const int DataSize()const noexcept { return m_writePos - m_readPos; }
	const int FreeSize()const noexcept { return m_capacity - m_writePos; }
private:
	const int m_bufferSize;
	const int m_capacity;
	int m_readPos = 0;
	int m_writePos = 0;
	char m_buffer[BUFFER_SIZE * BUFFER_COUNT];
};