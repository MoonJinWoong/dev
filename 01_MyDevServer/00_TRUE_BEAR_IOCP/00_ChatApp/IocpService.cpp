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

// TCP 연결 각각의 객체.
class Session
{
public:
	std::shared_ptr<std::thread> thread; // 클라이언트 처리를 하는 스레드 1개
	Socket tcpConnection;		// accept한 TCP 연결

	Session() {}
	Session(SocketType socketType) :tcpConnection(socketType) {}
};

std::unordered_map<Session*, std::shared_ptr<Session>> remoteClients;


IocpService::IocpService()
{
}

IocpService::~IocpService()
{
	IocpServiceShutDown();
}
void IocpService::ReadyIocp()
{
	// TCP 연결을 받는 소켓
	Socket listenSocket(SocketType::Tcp);

	// bind and listen
	listenSocket.Bind(Endpoint("0.0.0.0", 8000));
	listenSocket.Listen();

	// IOCP resister
	mIocp.Add(listenSocket, nullptr);

	// overlapped accept를 걸어둔다. 
	auto remoteClientCandidate = std::make_shared<Session>(SocketType::Tcp);

	std::string errorText;
	if (!listenSocket.AcceptOverlapped(remoteClientCandidate->tcpConnection, errorText)
		&& WSAGetLastError() != ERROR_IO_PENDING)
	{
		throw Exception("Overlapped AcceptEx failed.");
	}
	listenSocket.m_isReadOverlapped = true;

	std::cout << "서버 시작 준비 완료.\n";
}

bool IocpService::CreateThreads()
{
	for (int i = 0; i < mIocp.m_threadCount; ++i)
	{
		mIoThread.push_back(std::make_unique<std::thread>([&]() {IoThread(); }));
		std::cout << "  스레드가 돈다.\n";

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
	// 0917 
	// 밑에 빨간줄부터 다시 .... 
	while (true)
	{
				// I/O 완료 이벤트가 있을 때까지 기다립니다.
			IocpEvents readEvents;
			mIocp.GQCS(readEvents, 100);

			// 받은 이벤트 각각을 처리합니다.
			for (int i = 0; i < readEvents.m_eventCount; i++)
			{
				auto& readEvent = readEvents.m_events[i];
				if (readEvent.lpCompletionKey == 0) // 리슨소켓이면
				{					
					listenSocket.m_isReadOverlapped = false;
					// 이미 accept은 완료되었다. 귀찮지만, Win32 AcceptEx 사용법에 따르는 마무리 작업을 하자. 
					if (remoteClientCandidate->tcpConnection.UpdateAcceptContext(listenSocket) != 0)
					{
						//리슨소켓을 닫았던지 하면 여기서 에러날거다. 그러면 리슨소켓 불능상태로 만들자.
						listenSocket.Close();
					}
					else // 잘 처리함
					{
						shared_ptr<Session> remoteClient = remoteClientCandidate;

						// 새 TCP 소켓도 IOCP에 추가한다.
						iocp.Add(remoteClient->tcpConnection, remoteClient.get());

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

							cout << "Client joined. There are " << remoteClients.size() << " connections.\n";
						}

						// 계속해서 소켓 받기를 해야 하므로 리슨소켓도 overlapped I/O를 걸자.
						remoteClientCandidate = make_shared<Session>(SocketType::Tcp);
						string errorText;
						if (!listenSocket.AcceptOverlapped(remoteClientCandidate->tcpConnection, errorText)
							&& WSAGetLastError() != ERROR_IO_PENDING)
						{
							// 에러나면 리슨소켓 불능 상태로 남기자. 
							listenSocket.Close();
						}
						else
						{
							// 리슨소켓은 연결이 들어옴을 기다리는 상태가 되었다.
							listenSocket.m_isReadOverlapped = true;
						}
					}
				}
				else  // TCP 연결 소켓이면
				{
					// 받은 데이터를 그대로 회신한다.
					shared_ptr<Session> remoteClient = remoteClients[(Session*)readEvent.lpCompletionKey];

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
							ProcessClientLeave(remoteClient);
						}
						else
						{
							// 이미 수신된 상태이다. 수신 완료된 것을 그냥 꺼내 쓰자.
							char* echoData = remoteClient->tcpConnection.m_receiveBuffer;
							int echoDataLength = ec;

							// 원칙대로라면 TCP 스트림 특성상 일부만 송신하고 리턴하는 경우도 고려해야 하나,
							// 지금은 독자의 이해가 우선이므로, 생략하도록 한다.
							// 원칙대로라면 여기서 overlapped 송신을 해야 하지만 
							// 독자의 이해를 위해서 그냥 블로킹 송신을 한다.
							remoteClient->tcpConnection.Send(echoData, echoDataLength);

							// 다시 수신을 받으려면 overlapped I/O를 걸어야 한다.
							if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
								&& WSAGetLastError() != ERROR_IO_PENDING)
							{
								ProcessClientLeave(remoteClient);
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
}

void IocpService::IocpServiceShutDown()
{

}