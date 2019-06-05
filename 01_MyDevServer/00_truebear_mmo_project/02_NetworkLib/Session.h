#pragma once

#include "Singleton.h"


namespace NetworkLayer
{
	struct CLIENT_INFO
	{
		SOCKET s_sock;
		SOCKADDR_IN addrInfo;
	};
	typedef enum
	{
		READ,
		WRITE,
	}IO_OPERATION;

	struct SessionIO
	{
		OVERLAPPED over;
		IO_OPERATION type;
		unsigned int totalByte;
		unsigned int currentByte;

		std::array<char, SOCKET_BUF_SIZE> buffer;
		WSABUF wsabuf;

		bool remainSize;


		void clear();
	};

	class Session
	{
	public:
		Session();
		virtual ~Session();

		bool ProcAccept(SOCKET sock, SOCKADDR_IN addrLen);
		
		void ProcSend(size_t size);
		void SendData();


		std::array<SessionIO, MAX_IO_SIZE> ioData;

	private:

	};
}