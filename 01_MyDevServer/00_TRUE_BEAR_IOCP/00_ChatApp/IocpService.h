#pragma once
#include "..//01_NetLibrary/preComplie.h"

// TCP 연결 각각의 객체.
class Session
{
public:
	NetworkObject tcpConnection;		// accept한 TCP 연결
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


