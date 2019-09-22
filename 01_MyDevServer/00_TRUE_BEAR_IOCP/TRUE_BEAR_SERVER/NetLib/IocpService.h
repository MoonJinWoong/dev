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
	void DestorySessionList();

	bool CreateWorkThread();
	void WorkThread();

	void DoAcceptEx(const CustomOverlapped* pOver);
	void DoRecv(CustomOverlapped* pOver, const DWORD ioSize);

	Session* GetSession(const int sessionIdx);


private:
	// iocp 핸들을 두개 가지고 있음
	std::unique_ptr<Iocp> m_Iocp;

	// 소켓 , 바인드 , 리슨 
	std::unique_ptr<CustomSocket> m_ListenSock;
	
	std::unordered_map<int , Session*> m_SessionList;

	bool m_IsRunWorkThread = true;
	std::vector<std::unique_ptr<std::thread>> m_WorkerThreads;
};