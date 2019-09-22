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
	// ���� ���� ���� 
	m_ListenSock.CreateSocket();

	// bind and listen
	m_ListenSock.BindAndListenSocket();

	// �޼��� Ǯ ����


	// ���� ������ iocp�� ��� 
	m_Iocp.ResisterIocp(m_ListenSock.m_sock, nullptr);

	// ���� Ǯ ����
	CreateSessionList();

	// ��Ŀ ������ ���� 
	CreateWorkThread();


}

void IocpService::StopIocpService()
{
	if (m_Iocp.m_hIocp != INVALID_HANDLE_VALUE)
	{
		m_IsRunWorkThread = false;
		CloseHandle(m_Iocp.m_hIocp);

		for (int i = 0; i < m_WorkerThreads.size(); ++i)
		{
			if (m_WorkerThreads[i].get()->joinable())
			{
				m_WorkerThreads[i].get()->join();
			}
		}
	}

	if (m_ListenSock.m_sock != INVALID_SOCKET)
	{
		closesocket(m_ListenSock.m_sock);
		m_ListenSock.m_sock = INVALID_SOCKET;
	}

	WSACleanup();
}

bool IocpService::getNetworkMessage(INT8& msgType , INT32&  sessionIdx,char* pBuf , INT16& copySize )
{
	Message* pMsg = nullptr;
	Session* pSession = nullptr;
	DWORD ioSize = 0;


	if (!m_Iocp.GQCS(pMsg, pSession, ioSize))
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
		pSession->Init(m_ListenSock.m_sock, i);
		pSession->DoAcceptOverlapped();
		
		// �� ���ͷ� �ϸ� ������? 
		//m_SessionList.push_back(pSession);
		m_SessionList.insert({ i, pSession });
	}

	return true;
}

void IocpService::DestoryConnections()
{
	for (int i = 0; i < MAX_SESSION_COUNT; ++i)
	{
		//delete m_SessionList[i];
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
		DWORD ioSize = 0;
		CustomOverlapped* pOver = nullptr;
		Session* pSession = nullptr;


		auto result = GetQueuedCompletionStatus(
			m_Iocp.m_hIocp,
			&ioSize,
			reinterpret_cast<PULONG_PTR>(&pSession),
			reinterpret_cast<LPOVERLAPPED*>(&pOver),
			INFINITE);


		if (pOver == nullptr)
		{
			if (WSAGetLastError() != 0 && WSAGetLastError() != WSA_IO_PENDING)
			{
				std::cout << "GQCS in Worker fail..." <<WSAGetLastError()<< std::endl;
			}
			continue;
		}

		if (!result || (0 == ioSize && OPType::Accept != pOver->type))
		{
			// ű�� ���ǵ� ó�����־����
			//HandleExceptionWorkThread(pConnection, pOverlappedEx);
			continue;
		}

		switch (pOver->type)
		{
		case OPType::Accept:
			DoAccept(pOver);
			break;
		case OPType::Recv:
			//DoRecv(pOverlappedEx, ioSize);
			break;
		case OPType::Send:
			//DoSend(pOverlappedEx, ioSize);
			break;
		}
	}
}

Session* IocpService::GetSession(const INT32 sessionIdx)
{
	if (sessionIdx < 0 || sessionIdx >= MAX_SESSION_COUNT)
	{
		return nullptr;
	}
	auto iter = m_SessionList.find(sessionIdx);
	return iter->second;
}

void IocpService::DoAccept(const CustomOverlapped* pOverlappedEx)
{
	auto pConnection = GetSession(pOverlappedEx->SessionIdx);
	if (pConnection == nullptr)
	{
		return;
	}

	pConnection->DecrementAcceptIORefCount();

	
}