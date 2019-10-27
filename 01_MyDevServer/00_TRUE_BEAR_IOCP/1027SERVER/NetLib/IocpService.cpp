#include "IocpService.h"

IocpService::IocpService() 
{
	WSADATA w;
	WSAStartup(MAKEWORD(2, 2), &w);

	m_ListenSock = std::make_unique<CustomSocket>();
	m_MsgPool = std::make_unique<MessagePool>(MAX_MSG_POOL_COUNT, EXTRA_MSG_POOL_COUNT);
}

IocpService::~IocpService() 
{
	WSACleanup();
}

void IocpService::StartIocpService()
{
		// iocp port create
		mIOCP.CreateNewIocp();

		// listen socket 
		m_ListenSock->CreateSocket();

		// bind and listen
		m_ListenSock->BindAndListenSocket();

		// add device to iocp 
		mIOCP.AddDeviceListenSocket(m_ListenSock->mSock);

		// map container
		CreateSessionList();

		Sleep(500);

		// thread 
		CreateWorkThread();

}

void IocpService::StopIocpService()
{
	//TODO 일단 접속을 안받아야 하니까 리슨을 위로 먼저 올리는게 낫다
	if (m_ListenSock->mSock != INVALID_SOCKET)
	{
		closesocket(m_ListenSock->mSock);
		m_ListenSock->mSock = INVALID_SOCKET;
	}

	if (mIOCP.m_workIocp != INVALID_HANDLE_VALUE)
	{
		m_IsRunWorkThread = false;
		CloseHandle(mIOCP.m_workIocp);
		
		for (int i = 0; i < m_WorkerThreads.size(); ++i)
		{
			if (m_WorkerThreads[i].joinable())
			{
				m_WorkerThreads[i].join();
			}
		}
	}
	WSACleanup();
}

bool IocpService::CreateSessionList()
{
	//TODO 옵션값으로 받을 수 있게 바꾸자.
	for (int i = 0; i < MAX_SESSION_COUNT; ++i)
	{
		auto pSession = new Session(m_ListenSock->mSock,i);
		pSession->AcceptOverlapped();
		
		//TODO index로 할거니까 맵으로 굳이 할 필요없다. 
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
		mIOCP.GetCompletionEvents(events, 100);

		for (int i = 0; i < events.m_eventCount; i++)
		{
			auto& readEvent = events.m_IoArray[i];

			// AcceptEx
			if (readEvent.lpCompletionKey == 0) 
			{
				DoAccept(reinterpret_cast<CustomOverlapped*>(readEvent.lpOverlapped));
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
						DoRecv(Over,readEvent.dwNumberOfBytesTransferred);
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

void IocpService::DoAccept(const CustomOverlapped* pOverlappedEx)
{
	auto pSession = GetSession(pOverlappedEx->SessionIdx);
	if (pSession == nullptr)
	{
		return;
	}

	//pSession->DecrementAcceptIORefCount();


	// remote 주소 세팅
	if (!pSession->SetNetAddressInfo())
	{
		if (pSession->CloseComplete())
		{
			KickSession(pSession);
		}
		return;
	}

	mIOCP.AddDeviceRemoteSocket(pSession);

	pSession->UpdateSessionState();


	if (!pSession->PostRecv(pSession->RecvBufferBeginPos(), 0))
	{
		std::cout << "PostRecv Error" << std::endl;
		KickSession(pSession);
	}

	// TODO 여기에서 PCQS 도 던져준다.



	// 출력 확인용
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


	// echo 
	EchoSend(pSession, ioSize);


	pOver->OverlappedExWsaBuf.buf = pOver->pOverlappedExSocketMessage;
	pOver->OverlappedExWsaBuf.len = ioSize;
	pOver->OverlappedExRemainByte += ioSize;
	
	
	auto remainByte = pOver->OverlappedExRemainByte;
	char* pNext = NULL;
	pNext = pOver->OverlappedExWsaBuf.buf;

	RecvProcess(pSession, remainByte, pNext);


	if (!pSession->PostRecv(pNext, remainByte))
	{
		if (pSession->CloseComplete())
		{
			KickSession(pSession);
		}
	}

}

void IocpService::RecvProcess(Session* pSession, DWORD& remainByte, char* pBuffer)
{
	//TODO 아래 것은 로직에서 받아올 것
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
			if (!mIOCP.PostCompletionEvent(pSession, pMsg, currentSize))
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

void IocpService::EchoSend(Session *pSession, const DWORD ioSize)
{
	WSABUF b;
	b.buf = pSession->getRecvOverlappedEx()->OverlappedExWsaBuf.buf;
	b.len = ioSize;
	DWORD	sendBytes = 0;
	auto sendret = WSASend
	(
		pSession->GetRemoteSocket(),
		&b,
		1,
		&sendBytes,
		0,
		NULL,
		NULL
	);

	if (sendret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		std::cout << "fail EchoSend " << std::endl;
	}
}