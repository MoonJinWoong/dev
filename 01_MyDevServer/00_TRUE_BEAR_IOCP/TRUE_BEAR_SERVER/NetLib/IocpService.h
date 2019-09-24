#pragma once
#include "NetPreCompile.h"


class Session;
class IocpEvents;
class Iocp;


class IocpService
{
public:
	IocpService();
	~IocpService();
	void StartIocpService();
	void StopIocpService();

	bool GetNetworkMessage(
			INT8& msgType,
			INT32& sessionIdx,
			char* pBuf,
			INT16& copySize
		 );
	void PostNetMessage();

	bool CreateSessionList();
	void DestorySessionList();

	bool CreateWorkThread();
	void WorkThread();

	void DoAcceptEx(const CustomOverlapped* pOver);
	void DoRecv(CustomOverlapped* pOver, const DWORD ioSize);

	Session* GetSession(const int sessionIdx);

	// ������ ��������
	void KickSession(Session* pSession);

	// Ŭ�� ���� ����
	void DisConnectSession(Session* pSession, const CustomOverlapped* pOver);

private:
	// iocp �ڵ��� �ΰ� ������ ����
	std::unique_ptr<Iocp> m_Iocp;

	// ���� , ���ε� , ���� 
	std::unique_ptr<CustomSocket> m_ListenSock;
	
	std::unordered_map<int , Session*> m_SessionList;

	bool m_IsRunWorkThread = true;
	std::vector<std::thread> m_WorkerThreads;
};