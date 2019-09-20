#include "stdafx.h"
#include "..//01_NetLibrary/preComplie.h"
#include <stdio.h>
#include <signal.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <memory>
#include <vector>
#include <iostream>
#include <mutex>
#include "IocpService.h"



std::unordered_map<Session*, std::shared_ptr<Session>> remoteClients;


IocpService::IocpService()
{
}

IocpService::~IocpService()
{
}
void IocpService::ReadyIocp()
{
	// bind and listen
	mListenSocket.BindAndListen(Endpoint("0.0.0.0", 8000));

	// listen socket IOCP resister
	mIocp.ResisterIocp(mListenSocket, nullptr);

	// overlapped accept
	mRemoteClient = std::make_shared<Session>();
	std::string errorText;
	if (!mListenSocket.AcceptOverlapped(mRemoteClient->tcpConnection, errorText)
		&& WSAGetLastError() != ERROR_IO_PENDING)
	{
		throw Exception("Overlapped AcceptEx failed.");
	}
	mListenSocket.m_isReadOverlapped = true;

	std::cout << "서버 시작 준비 완료.\n";
}
void IocpService::ShutdownSession(std::shared_ptr<Session> remoteClient)
{
	remoteClient->tcpConnection.Close();
	remoteClients.erase(remoteClient.get());

	std::cout << "[leave Socket]:" << remoteClient->tcpConnection.m_fd << std::endl;
	std::cout << "[remain Session]:" << remoteClients.size() << std::endl;
}

bool IocpService::CreateThreads()
{
	// create thread
	for (int i = 0; i < mIocp.m_threadCount; ++i)
	{
		mIoThread.push_back(std::make_unique<std::thread>([&]() {IoThread(); }));
	}

	// join
	for (int i = 0; i < mIoThread.size(); ++i)
	{
		if (mIoThread[i].get()->joinable())
		{
			mIoThread[i].get()->join();
			continue;
		}
	}
	return true;
}

void IocpService::IoThread()
{
	
	while (true)
	{
		// GetQueuesCompletionStatus를 해준다.
		IocpEvents readEvents;
		mIocp.GQCS(readEvents, 100);


		// 받은 이벤트 각각을 처리
		for (int i = 0; i < readEvents.m_eventCount; i++)
		{
			auto& readEvent = readEvents.m_events[i];


			// accept면
			if (readEvent.lpCompletionKey == 0) 
			{		
				ProcessAcceptSession();	
			}
			else  // 연결된 소켓이면
			{
				// 받은 데이터를 그대로 회신한다.
				std::shared_ptr<Session> remoteClient = remoteClients[(Session*)readEvent.lpCompletionKey];

				if (readEvent.dwNumberOfBytesTransferred <= 0)
				{
					int a = 0;
				}

				if (remoteClient)
				{
					// 이미 수신된 상태이다. 수신 완료된 것을 그냥 꺼내 쓰자.
					remoteClient->tcpConnection.m_isReadOverlapped = false;
					int ec = readEvent.dwNumberOfBytesTransferred;

					if (ec <= 0)
					{ 
						// 읽은 결과가 0 즉 TCP 연결이 끝났다...
						// 혹은 음수 즉 뭔가 에러가 난 상태이다...
						ShutdownSession(remoteClient);
					}
					else
					{
						// 이미 수신된 상태이다. 수신 완료된 것을 그냥 꺼내 쓰자.
						char* echoData = remoteClient->tcpConnection.m_receiveBuffer;
						int echoDataLength = ec;

						auto tmp = remoteClient->tcpConnection.EchoSend(echoData, echoDataLength);

						// 다시 수신을 받으려면 overlapped I/O를 걸어야 한다.
						if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
							&& WSAGetLastError() != ERROR_IO_PENDING)
						{
							ShutdownSession(remoteClient);
						}
						else
						{
							// I/O를 걸었다. 완료를 대기하는 중 상태로 바꾸자.
							remoteClient->tcpConnection.m_isReadOverlapped = true;
						}
					}
				}
			}
		}
		
	}
}

void IocpService::ProcessAcceptSession()
{
	mListenSocket.m_isReadOverlapped = false;

	// accept 마무리까지 세팅해주어야 제대로 동작함
	if (mRemoteClient->tcpConnection.FinishAcceptEx(mListenSocket) != 0)
	{
		mListenSocket.Close();
	}
	else // 잘 처리함
	{
		std::shared_ptr<Session> remoteClient = mRemoteClient;

		// 새 TCP 소켓도 IOCP에 추가
		mIocp.ResisterIocp(remoteClient->tcpConnection, remoteClient.get());

		// overlapped 수신을 받을 수 있어야 하므로 여기서 I/O 수신 요청을 걸어둔다.
		if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
			&& WSAGetLastError() != ERROR_IO_PENDING)
		{
			// 에러. 소켓을 정리하자. 그리고 그냥 버리자.
			remoteClient->tcpConnection.Close();
		}
		else
		{
			// I/O를 걸었다. 완료를 대기하는 중 상태로 바꾸자.
			remoteClient->tcpConnection.m_isReadOverlapped = true;

			// 새 클라이언트를 목록에 추가.
			remoteClients.insert({ remoteClient.get(), remoteClient });

			std::cout << "Client joined. There are " << remoteClients.size() << " connections.\n";
		}

		// 계속해서 소켓 받기를 해야 하므로 리슨소켓도 overlapped I/O를 걸자.
		mRemoteClient = std::make_shared<Session>();
		std::string errorText;
		if (!mListenSocket.AcceptOverlapped(mRemoteClient->tcpConnection, errorText)
			&& WSAGetLastError() != ERROR_IO_PENDING)
		{
			// 에러나면 리슨소켓 불능 상태로 남기자. 
			mListenSocket.Close();
		}
		else
		{
			mListenSocket.m_isReadOverlapped = true;
		}
	}
}