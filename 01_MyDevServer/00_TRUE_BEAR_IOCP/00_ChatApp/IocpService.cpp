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

	std::cout << "���� ���� �غ� �Ϸ�.\n";
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
		// GetQueuesCompletionStatus�� ���ش�.
		IocpEvents readEvents;
		mIocp.GQCS(readEvents, 100);


		// ���� �̺�Ʈ ������ ó��
		for (int i = 0; i < readEvents.m_eventCount; i++)
		{
			auto& readEvent = readEvents.m_events[i];


			// accept��
			if (readEvent.lpCompletionKey == 0) 
			{		
				ProcessAcceptSession();	
			}
			else  // ����� �����̸�
			{
				// ���� �����͸� �״�� ȸ���Ѵ�.
				std::shared_ptr<Session> remoteClient = remoteClients[(Session*)readEvent.lpCompletionKey];

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
						ShutdownSession(remoteClient);
					}
					else
					{
						// �̹� ���ŵ� �����̴�. ���� �Ϸ�� ���� �׳� ���� ����.
						char* echoData = remoteClient->tcpConnection.m_receiveBuffer;
						int echoDataLength = ec;

						auto tmp = remoteClient->tcpConnection.EchoSend(echoData, echoDataLength);

						// �ٽ� ������ �������� overlapped I/O�� �ɾ�� �Ѵ�.
						if (remoteClient->tcpConnection.ReceiveOverlapped() != 0
							&& WSAGetLastError() != ERROR_IO_PENDING)
						{
							ShutdownSession(remoteClient);
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

void IocpService::ProcessAcceptSession()
{
	mListenSocket.m_isReadOverlapped = false;

	// accept ���������� �������־�� ����� ������
	if (mRemoteClient->tcpConnection.FinishAcceptEx(mListenSocket) != 0)
	{
		mListenSocket.Close();
	}
	else // �� ó����
	{
		std::shared_ptr<Session> remoteClient = mRemoteClient;

		// �� TCP ���ϵ� IOCP�� �߰�
		mIocp.ResisterIocp(remoteClient->tcpConnection, remoteClient.get());

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

			std::cout << "Client joined. There are " << remoteClients.size() << " connections.\n";
		}

		// ����ؼ� ���� �ޱ⸦ �ؾ� �ϹǷ� �������ϵ� overlapped I/O�� ����.
		mRemoteClient = std::make_shared<Session>();
		std::string errorText;
		if (!mListenSocket.AcceptOverlapped(mRemoteClient->tcpConnection, errorText)
			&& WSAGetLastError() != ERROR_IO_PENDING)
		{
			// �������� �������� �Ҵ� ���·� ������. 
			mListenSocket.Close();
		}
		else
		{
			mListenSocket.m_isReadOverlapped = true;
		}
	}
}