#pragma once
#include "NetPreCompile.h"


class Session;
class IocpService
{
public:
	IocpService();
	~IocpService();
	void StartIocpService();
	void StopIocpService();

	bool getNetworkMessage(
			INT8& msgType,
			INT32& sessionIdx,
			char* pBuf,
			INT16& copySize
		 );

	bool CreateSessionList();
	void DestoryConnections();

	bool CreateWorkThread();
	void WorkThread();

	void DoAccept(const CustomOverlapped* pOverlappedEx);
	Session* GetSession(const int sessionIdx);


private:
	Iocp m_Iocp;
	CustomSocket m_ListenSock;
	std::unordered_map<int , Session*> m_SessionList;

	bool m_IsRunWorkThread = true;
	std::vector<std::unique_ptr<std::thread>> m_WorkerThreads;
};