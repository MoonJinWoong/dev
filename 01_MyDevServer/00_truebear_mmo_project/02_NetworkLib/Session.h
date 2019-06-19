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

	// �������� Ȯ��
	struct ExOverIo
	{
		WSAOVERLAPPED	s_wsaOverlapped;		//Overlapped I/O����ü
		SOCKET			s_socketClient;			//Ŭ���̾�Ʈ ����
		WSABUF			s_wsaBuf;				//Overlapped I/O�۾� ����
		char			s_szBuf[MAX_IO_SIZE];   //������ ����
		IO_TYPE	s_eOperation;			//�۾� ���� ����

		void Clear()
		{
			
			ZeroMemory(s_szBuf, sizeof(char));
			s_eOperation = TYPE_NONE;
		}
		bool needMoreIo(size_t transferSize)
		{
			// ���⼭���� 
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
		SOCKET			m_socket;			//Cliet�� ����Ǵ� ����
		ExOverIo	m_stRecvIO;	//RECV Overlapped I/O�۾��� ���� ����
		ExOverIo	m_stSendIO;	//SEND Overlapped I/O�۾��� ���� ����
	};
}