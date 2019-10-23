#include "IocpService.h"

IocpService::IocpService() 
{
	WSADATA w;
	WSAStartup(MAKEWORD(2, 2), &w);

	m_Iocp = std::make_unique<Iocp>();
	m_ListenSock = std::make_unique<CustomSocket>();
	m_MsgPool = std::make_unique<MessagePool>(MAX_MSG_POOL_COUNT, EXTRA_MSG_POOL_COUNT);
}

IocpService::~IocpService() 
{
	WSACleanup();
}

void IocpService::StartIocpService()
{
	m_Iocp->CreateIocp();

	m_ListenSock->CreateSocket();

	m_ListenSock->BindAndListenSocket();

	m_Iocp->AddDeviceIocp(m_ListenSock->m_sock, nullptr);

	CreateSessionList();

	CreateWorkThread();
}

void IocpService::StopIocpService()
{
	if (m_Iocp->m_workIocp != INVALID_HANDLE_VALUE)
	{
		m_IsRunWorkThread = false;
		CloseHandle(m_Iocp->m_workIocp);

		for (int i = 0; i < m_WorkerThreads.size(); ++i)
		{
			if (m_WorkerThreads[i].joinable())
			{
				m_WorkerThreads[i].join();
			}
		}
	}

	if (m_ListenSock->m_sock != INVALID_SOCKET)
	{
		closesocket(m_ListenSock->m_sock);
		m_ListenSock->m_sock = INVALID_SOCKET;
	}
	WSACleanup();
}

bool IocpService::CreateSessionList()
{
	for (int i = 0; i < MAX_SESSION_COUNT; ++i)
	{
		auto pSession = new Session();
		pSession->Init(m_ListenSock->m_sock, i);
		pSession->AcceptOverlapped();
		
		m_SessionList.insert({ i, pSession });
	}

	return true;
}

void IocpService::DestorySessionList()
{
	for (int i = 0; i < MAX_SESSION_COUNT; ++i)
	{
		m_SessionList.erase(i);
	}
}

bool IocpService::CreateWorkThread()
{
	for (int i = 0; i < WORKER_THREAD_COUNT; ++i)
	{
		m_WorkerThreads.push_back(std::thread([&]() {WorkThread(); }));
	}
	return true;
}

void IocpService::WorkThread()
{
	while (m_IsRunWorkThread)
	{
		IocpEvents events;
		m_Iocp->GetCompletionEvents(events, 100);

		for (int i = 0; i < events.m_eventCount; i++)
		{
			auto& readEvent = events.m_IoArray[i];

			// AcceptEx
			if (readEvent.lpCompletionKey == 0) 
			{
				DoAcceptFinish(reinterpret_cast<CustomOverlapped*>(readEvent.lpOverlapped));
			}
			// Send , Recv 
			else
			{
				auto Over = reinterpret_cast<CustomOverlapped*>(readEvent.lpOverlapped);
				if (readEvent.dwNumberOfBytesTransferred <= 0)
				{
					// ���� �����־�� �ϴµ� ������ �ٲ�� �Ѵ�.
					// TODO �����غ� ��
				}
				
				switch (Over->type)
				{
					case OPType::Recv:
						DoRecvProcess(Over,readEvent.dwNumberOfBytesTransferred);
						break;
					case OPType::Send:
						//DoSend(Over, readEvent.dwNumberOfBytesTransferred);
						break;
				}
			}
		}
	}
}

Session* IocpService::GetSession(const int sessionIdx)
{
	auto iter = m_SessionList.find(sessionIdx);
	
	if (iter == m_SessionList.end())
	{
		return nullptr;
	}

	return iter->second;
}

void IocpService::DoAcceptFinish(const CustomOverlapped* pOverlappedEx)
{
	auto pSession = GetSession(pOverlappedEx->SessionIdx);
	if (pSession == nullptr)
	{
		return;
	}

	//pSession->DecrementAcceptIORefCount();


	// AcceptEx ������ �۾�
	if (pSession->SetNetAddressInfo() == false)
	{
		std::cout << "SetNetAddressInfo is failed..." << std::endl;
		if (pSession->CloseComplete())
		{
			KickSession(pSession);
		}
		return;
	}

	// �ش� ������ ���
	if (!pSession->BindIOCP(m_Iocp->m_workIocp))
	{
		if (pSession->CloseComplete())
		{
			KickSession(pSession);
		}
		return;
	}

	// ���� ���� ����
	pSession->UpdateSessionState();


	if (!pSession->PostRecv(pSession->RecvBufferBeginPos(), 0))
	{
		std::cout << "PostRecv Error" << std::endl;
		KickSession(pSession);
	}

	// TODO ���⿡�� PCQS �� �����ش�.



	// ��� Ȯ�ο�
	std::cout << "Session Join ["<<pSession->GetIndex()<<"]" << std::endl;
}

void IocpService::DoRecvProcess(CustomOverlapped* pOver, const DWORD ioSize)
{
	Session* pSession = GetSession(pOver->SessionIdx);
	if (pSession == nullptr)
	{
		return;
	}

	pSession->DecrementRecvIORefCount();


	// echo 
	EchoSend(pSession, ioSize);




	pOver->OverlappedExWsaBuf.buf = pOver->pOverlappedExSocketMessage;
	pOver->OverlappedExWsaBuf.len = ioSize;
	pOver->OverlappedExRemainByte += ioSize;
	
	
	auto remainByte = pOver->OverlappedExRemainByte;
	char* pNext = NULL;
	pNext = pOver->OverlappedExWsaBuf.buf;

	RecvFinish(pSession, remainByte, pNext);


	if (!pSession->PostRecv(pNext, remainByte))
	{
		if (pSession->CloseComplete())
		{
			KickSession(pSession);
		}
	}

}

void IocpService::RecvFinish(Session* pSession, DWORD& remainByte, char* pBuffer)
{
	
		const int PACKET_HEADER_LENGTH = 5;
		const int PACKET_SIZE_LENGTH = 2;
		const int PACKET_TYPE_LENGTH = 2;
		short packetSize = 0;


		while (true)
		{
			if (remainByte < PACKET_HEADER_LENGTH)
			{
				break;
			}

			//memcpy(&packetSize, &pBuffer, 2);
			CopyMemory(&packetSize, pBuffer, PACKET_SIZE_LENGTH);
			auto currentSize = packetSize;

			if (0 >= packetSize || packetSize > pSession->RecvBufferSize())
			{
				std::cout << "IOCPServer::DoRecv. Arrived Wrong Packet."<< std::endl;

				//pSession->DisConnectAsync();
				return;
			}

			if (remainByte >= (DWORD)currentSize)
			{
				auto pMsg = m_MsgPool->PopMsg();
				if (pMsg == nullptr)
				{
					return;
				}

				pMsg->SetMessage(MsgType::OnRecv, pBuffer);
				if (!m_Iocp->PQCSWorker(pSession, pMsg, currentSize))
				{
					m_MsgPool->PushMsg(pMsg);
					return;
				}
				remainByte -= currentSize;
				pBuffer += currentSize;
			}
			else
			{
				break;
			}
		}


}

void IocpService::KickSession(Session* pSession)
{

	if (pSession == nullptr)
	{
		return;
	}
	int idx = pSession->GetIndex();
	pSession->DisconnectSession();


	// �ٸ� �����鿡�� �޼����� �����־�� ��.
	//if (!PostNetMessage(pConnection, pConnection->GetCloseMsg()))
	//{
	//	pConnection->ResetConnection();
	//}
	std::cout << "Leave Session [" << idx << "]" << std::endl;

}

void IocpService::DisConnectSession(Session* pSession, const CustomOverlapped* pOver)
{
	if (pOver == nullptr)
	{
		return;
	}
	int idx = pSession->GetIndex();

	//Connection ���� ���� �� ���� IO ó��
	switch (pOver->type)
	{
	case OPType::Accept:
		//pSession->DecrementAcceptIORefCount();
		break;
	case OPType::Recv:
		//pSession->DecrementRecvIORefCount();
		break;
	case OPType::Send:
		//pSession->DecrementSendIORefCount();
		break;
	}

	if (pSession->CloseComplete())
	{
		KickSession(pSession);
	}
	return;
}

void IocpService::EchoSend(Session *pSession, const DWORD ioSize)
{
	WSABUF b;
	b.buf = pSession->m_pRecvOverlappedEx->OverlappedExWsaBuf.buf;
	b.len = ioSize;
	DWORD	sendBytes = 0;
	auto sendret = WSASend
	(
		pSession->m_ClientSocket,
		&b,
		1,
		&sendBytes,
		0,
		NULL,
		NULL
	);

	if (sendret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		printf_s("err WSASend fail : ", WSAGetLastError());
	}
}