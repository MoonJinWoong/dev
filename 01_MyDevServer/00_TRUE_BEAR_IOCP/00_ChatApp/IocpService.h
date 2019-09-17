#pragma once
#include "..//01_NetLibrary/preComplie.h"

// TCP ���� ������ ��ü.
class Session
{
public:
	NetworkObject tcpConnection;		// accept�� TCP ����
	Session() {}
};


class IocpService
{
public:
	IocpService();
	~IocpService();
	void ReadyIocp();
	bool CreateThreads();
	void IoThread();
	void ProcessAcceptSession();
	void ShutdownSession(std::shared_ptr<Session> remoteClient);

public:
	Iocp mIocp;
	NetworkObject mListenSocket;
	std::shared_ptr<Session> mRemoteClient;
	std::vector<std::unique_ptr<std::thread>> mIoThread;
};


