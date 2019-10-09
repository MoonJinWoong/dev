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


	//// ������ ��������
	//void KickSession(Session* pSession);

	//// Ŭ�� ���� ����
	//void DisConnectSession(Session* pSession, const CustomOverlapped* pOver);

private:
	// iocp �ڵ��� �ΰ� ������ ����
	Iocp mIocp;


	
	std::unordered_map<int , Session*> m_SessionList;

	bool m_IsRunWorkThread = true;
	std::vector<std::thread> m_WorkerThreads;


public:
	// ���� , ���ε� , ���� 
	CustomSocket m_ListenSock;
	// accept Ex remote �ּ� 
	static char mAcceptAddr[64] ;
};