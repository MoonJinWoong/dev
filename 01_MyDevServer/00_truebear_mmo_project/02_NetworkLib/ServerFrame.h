#pragma once

// �Ʒ��� ��������
//#include "SessionMgr.h"


namespace NetLib
{
	typedef enum STATUS
	{
		SERVERSTOP,
		SERVERINIT,
		SERVERREADY
	};

	class ServerFrame
	{
	public:
		ServerFrame();
		virtual ~ServerFrame();


		// TODO : initServerFrame�� ���� �Ϳ� ���� Start��
		// �ٸ��� ������ ���� 
		virtual void InitServerFrame();
		virtual bool StartServer() = 0;

		STATUS& status();

		


	protected:


		STATUS m_serverStatus;
		wchar_t m_ip[16];
		int m_port;
		int m_workerThreadCnt;
		int m_cpu_core;
	};
}
