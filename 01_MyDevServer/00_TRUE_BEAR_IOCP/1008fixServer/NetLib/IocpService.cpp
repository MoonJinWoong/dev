#include "IocpService.h"

char IocpService::mAcceptAddr[64] = { 0, };

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
	m_ListenSock.CreateSocket();

	m_ListenSock.BindAndListenSocket();

	//m_MsgPool->CheckCreate();

	mIocp.ResisterIocp(m_ListenSock.m_sock, nullptr);

	CreateSessionList();

	CreateWorkThread();
}

void IocpService::StopIocpService()
{
	if (mIocp.m_workIocp != INVALID_HANDLE_VALUE)
	{
		m_IsRunWorkThread = false;
		CloseHandle(mIocp.m_workIocp);

		for (int i = 0; i < m_WorkerThreads.size(); ++i)
		{
			if (m_WorkerThreads[i].joinable())
			{
				m_WorkerThreads[i].join();
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




bool IocpService::CreateSessionList()
{
	for (int i = 0; i < MAX_SESSION_COUNT; ++i)
	{
		// 소켓 만들고
	//	CustomOverlapped pCustomOverlapped = new CustomOverlapped()
		auto pSession = new Session(i);
		
		// accpet overlapped 걸어주고 
		pSession->AcceptOverlapped(m_ListenSock.m_sock , mAcceptAddr);


		std::cout << "create User  "
			<< "socket num : " << pSession->mSocket << std::endl;


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
		mIocp.GQCS_InWork(events, 100);

		for (int i = 0; i < events.m_eventCount; i++)
		{
			auto& readEvent = events.m_IoArray[i];

			// AcceptEx
			if (readEvent.lpCompletionKey == 0) 
			{
				// 여기 다시 
			}

			// Send , Recv 
			else
			{
				std::cout << "asdfasdf" << std::endl;

				//auto Over = reinterpret_cast<CustomOverlapped*>(readEvent.lpOverlapped);
				//if (readEvent.dwNumberOfBytesTransferred <= 0)
				//{
				//	// 세션 끊어주어야 하는데 구조를 바꿔야 한다.
				//	// TODO 생각해볼 것
				//}
				//
				//switch (Over->mIoType)
				//{
				//	case IOType::IO_RECV:
				//		std::cout << "recv call" << std::endl;
				//		//DoRecvProcess(Over,readEvent.dwNumberOfBytesTransferred);
				//		break;
				//	case IOType::IO_SEND:
				//		////DoSend(Over, readEvent.dwNumberOfBytesTransferred);
				//		break;
				//}
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


void IocpService::DoAcceptFinish(CustomOverlapped* pOver)
{
//	pOver->mSession
}

