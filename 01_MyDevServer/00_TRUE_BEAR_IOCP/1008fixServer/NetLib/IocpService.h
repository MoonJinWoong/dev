#pragma once
#include "NetPreCompile.h"


class Session;
class IocpEvents;
class Iocp;
class CustomOverlapped;


class IocpService
{
public:
	IocpService();
	~IocpService();
	void StartIocpService();
	void StopIocpService();

	//bool GetNetworkMsg(INT32& sessionIdx,char* pBuf,INT16& copySize);
	//bool PostNetworkMsg(Session* pSession, Message* pMsg, const DWORD packetSize = 0);



	bool CreateSessionList();
	void DestorySessionList();

	bool CreateWorkThread();
	void WorkThread();

	void DoAcceptFinish(CustomOverlapped* pOver);
	//
	//
	//void DoRecvProcess(CustomOverlapped* pOver, const DWORD ioSize);
	//void RecvFinish(Session* pSession, DWORD& remainByte, char* pBuffer);

	Session* GetSession(const int sessionIdx);


	//// 서버가 보내버림
	//void KickSession(Session* pSession);

	//// 클라가 접속 끊음
	//void DisConnectSession(Session* pSession, const CustomOverlapped* pOver);

private:
	// iocp 핸들을 두개 가지고 있음
	Iocp mIocp;


	
	std::unordered_map<int , Session*> m_SessionList;

	bool m_IsRunWorkThread = true;
	std::vector<std::thread> m_WorkerThreads;


public:
	// 소켓 , 바인드 , 리슨 
	CustomSocket m_ListenSock;
	// accept Ex remote 주소 
	static char mAcceptAddr[64] ;
};