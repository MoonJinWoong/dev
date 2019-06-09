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

	// �������� Ȯ��
	struct SessionIO
	{
		WSAOVERLAPPED	s_wsaOverlapped;		//Overlapped I/O����ü
		SOCKET			s_socketClient;			//Ŭ���̾�Ʈ ����
		WSABUF			s_wsaBuf;				//Overlapped I/O�۾� ����
		char			s_szBuf[MAX_IO_SIZE];   //������ ����
		IO_OPERATION	s_eOperation;			//�۾� ���� ����
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

		SOCKET			m_socket;			//Cliet�� ����Ǵ� ����
		SessionIO	m_stRecvIO;	//RECV Overlapped I/O�۾��� ���� ����
		SessionIO	m_stSendIO;	//SEND Overlapped I/O�۾��� ���� ����


	};
}