#include "preCompile.h"

#include "ServerOpt.h"
#include "ServerFrame.h"



namespace Core
{
	ServerFrame::ServerFrame() 
	{
		m_port = SERVERPORT;
		memcpy(m_ip, IP, sizeof(IP));
		m_serverStatus = SERVERINIT;
		m_cpu_core = CPU_CORE;
	}
	ServerFrame::~ServerFrame() {}

	void ServerFrame::InitServerFrame()
	{
		// 상속을 쓰면 생성자에서 초기화를 해주는 것이 더 낫네
		//m_port = SERVERPORT;
		//memcpy(m_ip, IP, sizeof(IP));
		//m_serverStatus = SERVERINIT;
	}
}