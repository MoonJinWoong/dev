#pragma once

// 아래놈 만들어야함
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


		// TODO : initServerFrame에 들어온 것에 따라 Start를
		// 다르게 구현할 예정 
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
