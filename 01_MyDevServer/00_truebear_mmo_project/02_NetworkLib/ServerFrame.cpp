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
		// ����� ���� �����ڿ��� �ʱ�ȭ�� ���ִ� ���� �� ����
		//m_port = SERVERPORT;
		//memcpy(m_ip, IP, sizeof(IP));
		//m_serverStatus = SERVERINIT;
	}
}