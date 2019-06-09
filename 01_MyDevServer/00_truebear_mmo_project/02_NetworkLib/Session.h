#pragma once

//#include "Singleton.h"
//#include "ServerOpt.h"
#include "preCompile.h"

namespace NetworkLayer
{
	typedef enum
	{
		NONE,
		READ,
		WRITE,
	}IO_OPERATION;

	// 오버랩드 확장
	struct SessionIO
	{
		WSAOVERLAPPED	s_wsaOverlapped;		//Overlapped I/O구조체
		SOCKET			s_socketClient;			//클라이언트 소켓
		WSABUF			s_wsaBuf;				//Overlapped I/O작업 버퍼
		char			s_szBuf[MAX_IO_SIZE];   //데이터 버퍼
		IO_OPERATION	s_eOperation;			//작업 동작 종류
	};

	class Session
	{
	public:
		Session();
		virtual ~Session();
		void Init();

		bool OnAccept(SOCKET sock, SOCKADDR_IN addrLen);
		
		void ProcSend(size_t size);
		void SendData();


		SessionIO m_IoData;

		void setUniqueId(const long long& id) { m_Unique_id = id; }
		long long getUniqueId() const { return m_Unique_id; }


		

	public:

	
		long long m_Unique_id{ 0 };

		SOCKET			m_socket;			//Cliet와 연결되는 소켓
		SessionIO	m_stRecvIO;	//RECV Overlapped I/O작업을 위한 변수
		SessionIO	m_stSendIO;	//SEND Overlapped I/O작업을 위한 변수


	};
}