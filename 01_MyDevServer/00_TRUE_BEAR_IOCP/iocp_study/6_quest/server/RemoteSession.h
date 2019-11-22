#pragma once
#include "TypeDefine.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#include <iostream>
#include <queue>

const int MAX_SOCKBUF = 1024;	//패킷 크기
const int MAX_WORKERTHREAD = 4;  //쓰레드 풀에 넣을 쓰레드 수

enum class IOOperation
{
	RECV,
	SEND
};

//WSAOVERLAPPED구조체를 확장
struct CustomOverEx
{
	WSAOVERLAPPED m_wsaOverlapped;		//Overlapped I/O구조체
	SOCKET		m_socketClient;			//클라이언트 소켓
	WSABUF		m_wsaBuf;				//Overlapped I/O작업 버퍼

	char		m_RecvBuf[MAX_SOCKBUF]; //데이터 버퍼
	char		m_SendBuf[MAX_SOCKBUF]; //데이터 버퍼
	IOOperation m_eOperation;			//작업 동작 종류

};

// 원격 세션
class RemoteSession
{
public:
	RemoteSession();

	SOCKET& GetSock() { return mRemoteSock; }

	bool SendPushInLogic(c_u_Int size, char* pMsg);
	void SendPopInWorker();
	void SendMsg();

	bool RecvMsg();

	void SetUniqueId(c_u_Int& id) { unique_id = id; }
	u_Int GetUniqueId() const { return unique_id; }

	SOCKET						mRemoteSock;			//Cliet와 연결되는 소켓
private:

	CustomOverEx				mRecvOver;	//RECV Overlapped I/O작업을 위한 변수
	//CustomOverEx				mSendOver;	//SEND Overlapped I/O작업을 위한 변수
	
	std::mutex					mSendLock;
	std::queue<CustomOverEx*>	mSendQ;

	unsigned int unique_id = -1;
};