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
	SEND,
	ACCEPT
};

//WSAOVERLAPPED구조체를 확장
struct CustomOverEx
{
	WSAOVERLAPPED	 m_wsaOverlapped;		
	WSABUF			 m_wsaBuf;				
	u_Int			 mUid = -1;			// unique id

	char		m_RecvBuf[MAX_SOCKBUF]; //데이터 버퍼
	char		m_SendBuf[MAX_SOCKBUF]; //데이터 버퍼
	IOOperation m_eOperation;			//작업 동작 종류

};

// 원격 세션
class RemoteSession
{
public:
	RemoteSession();

	bool AccpetAsync(SOCKET listenSock);
	bool AcceptFinish(HANDLE mhIocp);

	bool SendPushInLogic(c_u_Int size, char* pMsg);
	void SendPopInWorker();
	void SendMsg();

	bool RecvZeroByte();
	bool RecvStart();

	void SetUniqueId(c_u_Int& id) { mIdx = id; }
	u_Int GetUniqueId() const { return mIdx; }

	void SetConnected() { mIsConnected = true; }
	bool IsConnected() const { return mIsConnected; }

	void Release(bool IsForce,SOCKET mListenSock);

	SOCKET						mRemoteSock;			//Client와 연결되는 소켓
	SOCKET& GetSock() { return mRemoteSock; }

private:
	CustomOverEx				mOverIo;		// I/O작업
	CustomOverEx				mOverAccept;	// acceptEx 작업 
	char						mAcceptBuf[64];
	
	std::mutex					mSendLock;
	std::queue<CustomOverEx*>	mSendQ;

	bool						mIsConnected = false;
	u_Int						mIdx = -1;
};