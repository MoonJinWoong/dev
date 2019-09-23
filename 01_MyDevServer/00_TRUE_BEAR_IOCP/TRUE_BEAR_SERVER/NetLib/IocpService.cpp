#include "IocpService.h"

IocpService::IocpService() 
{
	WSADATA w;
	WSAStartup(MAKEWORD(2, 2), &w);
}

IocpService::~IocpService() 
{
	WSACleanup();
}

void IocpService::StartIocpService()
{
	m_Iocp = std::make_unique<Iocp>();
	m_ListenSock = std::make_unique<CustomSocket>();
	
	// ���� ���� ���� 
	m_ListenSock->CreateSocket();

	// bind and listen
	m_ListenSock->BindAndListenSocket();

	// �޼��� Ǯ ����


	// ���� ������ iocp�� ��� 
	m_Iocp->ResisterIocp(m_ListenSock->m_sock, nullptr);

	// ���� Ǯ ����
	CreateSessionList();

	// ��Ŀ ������ ���� 
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
			if (m_WorkerThreads[i].get()->joinable())
			{
				m_WorkerThreads[i].get()->join();
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

bool IocpService::getNetworkMessage(INT8& msgType , INT32&  sessionIdx,char* pBuf , INT16& copySize )
{
	Message* pMsg = nullptr;
	Session* pSession = nullptr;
	DWORD ioSize = 0;


	if (!m_Iocp->GQCS(pMsg, pSession, ioSize))
	{
		return false;
	}

	switch (pMsg->Type)
	{
	case MsgType::Session:
		//DoPostConnection(pConnection, pMsg, msgOperationType, connectionIndex);
		break;
	case MsgType::Close:
		//TODO �� ��뿡 �����̸� �ֵ��� �Ѵ�. ������ �� ������� ���� ���� IOCP ��ũ �����忡�� �� ������ ȣ��� ���� �ִ�. 
	//	DoPostClose(pConnection, pMsg, msgOperationType, connectionIndex);
		break;
	case MsgType::OnRecv:
		//DoPostRecvPacket(pConnection, pMsg, msgOperationType, connectionIndex, pBuf, copySize, ioSize);
	//	m_pMsgPool->DeallocMsg(pMsg);
		break;
	}


	return true;
}

bool IocpService::CreateSessionList()
{
	for (int i = 0; i < MAX_SESSION_COUNT; ++i)
	{
		auto pSession = new Session();
		pSession->Init(m_ListenSock->m_sock, i);
		pSession->DoAcceptOverlapped();
		
		// �� ���ͷ� �ϸ� ������? 
		//m_SessionList.push_back(pSession);
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
		m_WorkerThreads.push_back(std::make_unique<std::thread>([&]() {WorkThread(); }));
	}
	return true;
}

void IocpService::WorkThread()
{
	while (m_IsRunWorkThread)
	{
		IocpEvents events;
		m_Iocp->GQCSInWorker(events, 100);

		for (int i = 0; i < events.m_eventCount; i++)
		{
			auto& readEvent = events.m_IoArray[i];

			// AcceptEx
			if (readEvent.lpCompletionKey == 0) 
			{
				DoAcceptEx((CustomOverlapped*)readEvent.lpOverlapped);
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
						DoRecv(Over,readEvent.dwNumberOfBytesTransferred);
						break;
					case OPType::Send:
						DoRecv(Over, readEvent.dwNumberOfBytesTransferred);
						break;
				}
			}
		}
	}
}

Session* IocpService::GetSession(const int sessionIdx)
{
	if (sessionIdx < 0 || sessionIdx >= MAX_SESSION_COUNT)
	{
		return nullptr;
	}
	auto iter = m_SessionList.find(sessionIdx);
	return iter->second;
}

void IocpService::DoAcceptEx(const CustomOverlapped* pOverlappedEx)
{
	auto pSession = GetSession(pOverlappedEx->SessionIdx);
	if (pSession == nullptr)
	{
		return;
	}

	pSession->DecrementAcceptIORefCount();


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

	// PCQS �� �����ش�.
	// �ٸ� ���ǵ鵵 �޼��� �ް�


	// ��� Ȯ�ο�
	std::cout << "Session Join ["<<pSession->GetIndex()<<"]" << std::endl;
}

void IocpService::DoRecv(CustomOverlapped* pOver, const DWORD ioSize)
{
	Session* pSession = GetSession(pOver->SessionIdx);
	if (pSession == nullptr)
	{
		return;
	}

	pSession->DecrementRecvIORefCount();

	pOver->OverlappedExWsaBuf.buf = pOver->pOverlappedExSocketMessage;
	pOver->OverlappedExRemainByte += ioSize;

	auto remainByte = pOver->OverlappedExRemainByte;
	auto pNext = pOver->OverlappedExWsaBuf.buf;

	//PacketForwardingLoop(pConnection, remainByte, pNext);

	if (!pSession->PostRecv(pNext, remainByte))
	{
		if (pSession->CloseComplete())
		{
			KickSession(pSession);
		}
	}


	// echo send  �ӽ� �ڵ� ���߿� �������� . ������ ���� ���� 
	WSABUF b;
	b.buf = pOver->pOverlappedExSocketMessage;
	b.len = ioSize;
	int m_writeFlag = 0;
	DWORD sendByte = 0;

	auto ret = WSASend(
		pSession->GetClientSocket(),
		&b,
		1,
		&sendByte,
		m_writeFlag,
		NULL,
		NULL
	);
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
		pSession->DecrementAcceptIORefCount();
		break;
	case OPType::Recv:
		pSession->DecrementRecvIORefCount();
		break;
	case OPType::Send:
		pSession->DecrementSendIORefCount();
		break;
	}

	if (pSession->CloseComplete())
	{
		KickSession(pSession);
	}
	return;
}
