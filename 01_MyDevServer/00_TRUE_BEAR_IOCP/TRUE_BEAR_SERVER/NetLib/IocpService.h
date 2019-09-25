#pragma once
#include "NetPreCompile.h"


class Session;
class MessagePool;
class IocpEvents;
class Iocp;


class IocpService
{
public:
	IocpService();
	~IocpService();
	void StartIocpService();
	void StopIocpService();

	bool GetNetworkMsg(INT32& sessionIdx,char* pBuf,INT16& copySize);
	bool PostNetworkMsg(Session* pSession, Message* pMsg, const DWORD packetSize = 0);
	void ProcessPacket(Session* pSession, DWORD& remainByte, char* pBuffer);



	bool CreateSessionList();
	void DestorySessionList();

	bool CreateWorkThread();
	void WorkThread();

	void DoAcceptEx(const CustomOverlapped* pOver);
	void DoRecv(CustomOverlapped* pOver, const DWORD ioSize);

	Session* GetSession(const int sessionIdx);

	// 서버가 보내버림
	void KickSession(Session* pSession);

	// 클라가 접속 끊음
	void DisConnectSession(Session* pSession, const CustomOverlapped* pOver);

private:
	// iocp 핸들을 두개 가지고 있음
	std::unique_ptr<Iocp> m_Iocp;

	// 소켓 , 바인드 , 리슨 
	std::unique_ptr<CustomSocket> m_ListenSock;
	
	std::unordered_map<int , Session*> m_SessionList;

	bool m_IsRunWorkThread = true;
	std::vector<std::thread> m_WorkerThreads;

	std::unique_ptr<MessagePool> m_MsgPool;
};