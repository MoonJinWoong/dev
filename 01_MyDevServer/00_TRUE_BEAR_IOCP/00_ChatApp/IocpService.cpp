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

// TCP ���� ������ ��ü.
class Session
{
public:
	std::shared_ptr<std::thread> thread; // Ŭ���̾�Ʈ ó���� �ϴ� ������ 1��
	Socket tcpConnection;		// accept�� TCP ����

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
	// TCP ������ �޴� ����
	Socket listenSocket(SocketType::Tcp);

	// bind and listen
	listenSocket.Bind(Endpoint("0.0.0.0", 8000));
	listenSocket.Listen();

	// IOCP resister
	mIocp.Add(listenSocket, nullptr);

	// overlapped accept�� �ɾ�д�. 
	auto remoteClientCandidate = std::make_shared<Session>(SocketType::Tcp);

	std::string errorText;
	if (!listenSocket.AcceptOverlapped(remoteClientCandidate->tcpConnection, errorText)
		&& WSAGetLastError() != ERROR_IO_PENDING)
	{
		throw Exception("Overlapped AcceptEx failed.");
	}
	listenSocket.m_isReadOverlapped = true;

	std::cout << "���� ���� �غ� �Ϸ�.\n";
}

bool IocpService::CreateThreads()
{
	for (int i = 0; i < mIocp.m_threadCount; ++i)
	{
		mIoThread.push_back(std::make_unique<std::thread>([&]() {IoThread(); }));
		std::cout << "  �����尡 ����.\n";

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
	// �ؿ� �����ٺ��� �ٽ� .... 
	while (true)
	{
				// I/O �Ϸ� �̺�Ʈ�� ���� ������ ��ٸ��ϴ�.
			IocpEvents readEvents;
			mIocp.GQCS(readEvents, 100);

			// ���� �̺�Ʈ ������ ó���մϴ�.
			for (int i = 0; i < readEvents.m_eventCount; i++)
			{
				auto& readEvent = readEvents.m_events[i];
				if (readEvent.lpCompletionKey == 0) // ���������̸�
				{					
					listenSocket.m_isReadOverlapped = false;
					// �̹� accept�� �Ϸ�Ǿ���. ��������, Win32 AcceptEx ������ ������ ������ �۾��� ����. 
					if (remoteClientCandidate->tcpConnection.UpdateAcceptContext(listenSocket) != 0)
					{
						//���������� �ݾҴ��� �ϸ� ���⼭ �������Ŵ�. �׷��� �������� �Ҵɻ��·� ������.
						listenSocket.Close();
					}
					else // �� ó����
					{
						shared_ptr<Session> remoteClient = remoteClientCandidate;

						// �� TCP ���ϵ� IOCP�� �߰��Ѵ�.
						iocp.Add(remoteClient->tcpConnection, remoteClient.get());

						// overlapped ������ ���� �� �־�� �ϹǷ� ���⼭ I/O ���� ��û�� �ɾ�д�.
						if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
							&& WSAGetLastError() != ERROR_IO_PENDING)
						{
							// ����. ������ ��������. �׸��� �׳� ������.
							remoteClient->tcpConnection.Close();
						}
						else
						{
							// I/O�� �ɾ���. �ϷḦ ����ϴ� �� ���·� �ٲ���.
							remoteClient->tcpConnection.m_isReadOverlapped = true;

							// �� Ŭ���̾�Ʈ�� ��Ͽ� �߰�.
							remoteClients.insert({ remoteClient.get(), remoteClient });

							cout << "Client joined. There are " << remoteClients.size() << " connections.\n";
						}

						// ����ؼ� ���� �ޱ⸦ �ؾ� �ϹǷ� �������ϵ� overlapped I/O�� ����.
						remoteClientCandidate = make_shared<Session>(SocketType::Tcp);
						string errorText;
						if (!listenSocket.AcceptOverlapped(remoteClientCandidate->tcpConnection, errorText)
							&& WSAGetLastError() != ERROR_IO_PENDING)
						{
							// �������� �������� �Ҵ� ���·� ������. 
							listenSocket.Close();
						}
						else
						{
							// ���������� ������ ������ ��ٸ��� ���°� �Ǿ���.
							listenSocket.m_isReadOverlapped = true;
						}
					}
				}
				else  // TCP ���� �����̸�
				{
					// ���� �����͸� �״�� ȸ���Ѵ�.
					shared_ptr<Session> remoteClient = remoteClients[(Session*)readEvent.lpCompletionKey];

					if (readEvent.dwNumberOfBytesTransferred <= 0)
					{
						int a = 0;
					}


					if (remoteClient)
					{
						// �̹� ���ŵ� �����̴�. ���� �Ϸ�� ���� �׳� ���� ����.
						remoteClient->tcpConnection.m_isReadOverlapped = false;
						int ec = readEvent.dwNumberOfBytesTransferred;

						if (ec <= 0)
						{ 
							// ���� ����� 0 �� TCP ������ ������...
							// Ȥ�� ���� �� ���� ������ �� �����̴�...
							ProcessClientLeave(remoteClient);
						}
						else
						{
							// �̹� ���ŵ� �����̴�. ���� �Ϸ�� ���� �׳� ���� ����.
							char* echoData = remoteClient->tcpConnection.m_receiveBuffer;
							int echoDataLength = ec;

							// ��Ģ��ζ�� TCP ��Ʈ�� Ư���� �Ϻθ� �۽��ϰ� �����ϴ� ��쵵 ����ؾ� �ϳ�,
							// ������ ������ ���ذ� �켱�̹Ƿ�, �����ϵ��� �Ѵ�.
							// ��Ģ��ζ�� ���⼭ overlapped �۽��� �ؾ� ������ 
							// ������ ���ظ� ���ؼ� �׳� ���ŷ �۽��� �Ѵ�.
							remoteClient->tcpConnection.Send(echoData, echoDataLength);

							// �ٽ� ������ �������� overlapped I/O�� �ɾ�� �Ѵ�.
							if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
								&& WSAGetLastError() != ERROR_IO_PENDING)
							{
								ProcessClientLeave(remoteClient);
							}
							else
							{
								// I/O�� �ɾ���. �ϷḦ ����ϴ� �� ���·� �ٲ���.
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