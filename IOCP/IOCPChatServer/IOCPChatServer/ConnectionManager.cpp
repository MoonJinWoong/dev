#include "preCompile.h"



ConnectionManager::ConnectionManager()
{
	InitializeConnection();
}

ConnectionManager::~ConnectionManager()
{

}

void ConnectionManager::InitializeConnection()
{
	//ZeroMemory(m_szIp, MAX_IP_LENGTH);
	//m_socket = INVALID_SOCKET;
	//m_bIsConnect = FALSE;
	//m_bIsClosed = FALSE;
	//m_bIsSend = TRUE;
	//m_dwSendIoRefCount = 0;
	//m_dwRecvIoRefCount = 0;
	//m_dwAcceptIoRefCount = 0;

	cRecvCircleBuffer.InitializeCircleBuffer();
	cSendCircleBuffer.InitializeCircleBuffer();
}