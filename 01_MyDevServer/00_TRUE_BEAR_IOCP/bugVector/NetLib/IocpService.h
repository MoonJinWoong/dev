#pragma once
//#include "NetPreCompile.h"
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <iostream>

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

#include "Defines.h"
#include "MessagePool.h"
#include "CustomSocket.h"
#include "Session.h"			// 통신 모음
#include "Iocp.h"				// iocp 함수 모음

class CustomSocket;
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

	//bool GetNetworkMsg(INT32& sessionIdx,char* pBuf,INT16& copySize);
	//bool PostNetworkMsg(Session* pSession, Message* pMsg, const DWORD packetSize = 0);

	bool CreateSessionList();
	void DestorySessionList();

	bool CreateWorkThread();
	void WorkThread();

	void DoAccept(const CustomOverlapped* pOver);
	
	
	void DoRecv(CustomOverlapped* pOver, const unsigned long ioSize);
	void RecvProcess(Session* pSession, unsigned long& remainByte, char* pBuffer);

	Session* GetSession(const int sessionIdx);

	// 서버가 보내버림
	void KickSession(Session* pSession);

	// 클라가 접속 끊음
	void DisConnectSession(Session* pSession, const CustomOverlapped* pOver);

	void EchoSend(Session* pSession, const unsigned long ioSize);
private:
	Iocp mIOCP;

	// 소켓 , 바인드 , 리슨 
	std::unique_ptr<CustomSocket> m_ListenSock;
	
	std::vector<Session*> m_SessionList;

	bool m_IsRunWorkThread = true;
	std::vector<std::thread> m_WorkerThreads;

	std::unique_ptr<MessagePool> m_MsgPool;
};