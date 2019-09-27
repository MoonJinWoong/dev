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
	m_ListenSock->CreateSocket();

	m_ListenSock->BindAndListenSocket();

	//m_MsgPool->CheckCreate();

	m_Iocp->ResisterIocp(m_ListenSock->m_sock, nullptr);

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

bool IocpService::GetNetworkMsg( INT32&  sessionIdx,char* pBuf , INT16& copySize )
{
	Message* pMsg = nullptr;
	Session* pSession = nullptr;
	DWORD ioSize = 0;

	bool ret = m_Iocp->GQCS_InLogic(pSession, pMsg);
	if (!ret)
	{
		std::cout << "in logic .. GQCS fail" << std::endl;
		return false;
	}

	switch (pMsg->Type)
	{
	case MsgType::Session:
		std::cout << "Session input Success" << std::endl;
		//DoPostConnection(pConnection, pMsg, msgOperationType, connectionIndex);
		break;
	case MsgType::Close:
		//TODO 재 사용에 딜레이를 주도록 한다. 이유는 재 사용으로 가능 도중 IOCP 워크 스레드에서 이 세션이 호출될 수도 있다. 
		//DoPostClose(pConnection, pMsg, msgOperationType, connectionIndex);
		break;
	case MsgType::OnRecv:
		//	DoPostRecvPacket(pConnection, pMsg, msgOperationType, connectionIndex, pBuf, copySize, ioSize);
		break;
	}

	// 메세지큐에 집어넣는다. 
	m_MsgPool->PushMsg(pMsg);

	return true;
}


bool IocpService::PostNetworkMsg(Session* pSession, Message* pMsg, const DWORD packetSize)
{
	if (m_Iocp->m_logicIocp == INVALID_HANDLE_VALUE || pMsg == nullptr)
	{
		std::cout << "PostNetworkMsg fail" << std::endl;
		return false;
	}

	auto ret = m_Iocp->PQCSWorker(pSession, pMsg, packetSize);

	if (!ret)
	{
		std::cout << "PostQueuedCompletionStatus fail" << std::endl;
		return false;
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
		m_Iocp->GQCS_InWork(events, 100);

		for (int i = 0; i < events.m_eventCount; i++)
		{
			auto& readEvent = events.m_IoArray[i];

			// AcceptEx
			if (readEvent.lpCompletionKey == 0) 
			{
				DoAcceptEx(reinterpret_cast<CustomOverlapped*>(readEvent.lpOverlapped));
			}
			// Send , Recv 
			else
			{
				auto Over = reinterpret_cast<CustomOverlapped*>(readEvent.lpOverlapped);
				if (readEvent.dwNumberOfBytesTransferred <= 0)
				{
					// 세션 끊어주어야 하는데 구조를 바꿔야 한다.
					// TODO 생각해볼 것
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

void IocpService::DoAcceptEx(const CustomOverlapped* pOverlappedEx)
{
	auto pSession = GetSession(pOverlappedEx->SessionIdx);
	if (pSession == nullptr)
	{
		return;
	}

	//pSession->DecrementAcceptIORefCount();


	// AcceptEx 마무리 작업
	if (pSession->SetNetAddressInfo() == false)
	{
		std::cout << "SetNetAddressInfo is failed..." << std::endl;
		if (pSession->CloseComplete())
		{
			KickSession(pSession);
		}
		return;
	}

	// 해당 소켓을 등록
	if (!pSession->BindIOCP(m_Iocp->m_workIocp))
	{
		if (pSession->CloseComplete())
		{
			KickSession(pSession);
		}
		return;
	}

	// 세션 상태 갱신
	pSession->UpdateSessionState();


	if (!pSession->PostRecv(pSession->RecvBufferBeginPos(), 0))
	{
		std::cout << "PostRecv Error" << std::endl;
		KickSession(pSession);
	}

	// TODO 여기에서 PCQS 도 던져준다.
	// 다른 세션들도 메세지 받게 




	// 출력 확인용
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

	pOver->OverlappedExWsaBuf.buf = pOver->pOverlappedExSocketMessage;
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


	// echo send
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


	// 다른 유저들에게 메세지를 던져주어야 함.
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

	//Connection 접속 종료 시 남은 IO 처리
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
