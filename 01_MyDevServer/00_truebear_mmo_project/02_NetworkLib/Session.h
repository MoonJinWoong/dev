#pragma once

//#include "Singleton.h"
//#include "ServerOpt.h"
#include "preCompile.h"

namespace Network
{
	typedef enum
	{
		TYPE_NONE,
		TYPE_READ,
		TYPE_WRITE,
		TYPE_ERROR
	}IO_TYPE;

	// 오버랩드 확장
	struct ExOverIo
	{
		WSAOVERLAPPED	s_wsaOverlapped;		//Overlapped I/O구조체
		SOCKET			s_socketClient;			//클라이언트 소켓
		WSABUF			s_wsaBuf;				//Overlapped I/O작업 버퍼
		char			s_szBuf[MAX_IO_SIZE];   //데이터 버퍼
		IO_TYPE	s_eOperation;			//작업 동작 종류

		void Clear()
		{
			
			ZeroMemory(s_szBuf, sizeof(char));
			s_eOperation = TYPE_NONE;
		}
		bool needMoreIo(size_t transferSize)
		{
			// 여기서부터 
		}
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

		ExOverIo m_IoData;

		void setUniqueId(const long long& id) { m_Unique_id = id; }
		long long getUniqueId() const { return m_Unique_id; }

	public:
		long long m_Unique_id{ 0 };
		SOCKET			m_socket;			//Cliet와 연결되는 소켓
		ExOverIo	m_stRecvIO;	//RECV Overlapped I/O작업을 위한 변수
		ExOverIo	m_stSendIO;	//SEND Overlapped I/O작업을 위한 변수
	};
}