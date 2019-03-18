#pragma once


const int SERVERPORT	= 9000;
const int BUFSIZE		= 512;
const int MAX_USER		= 100;

typedef struct _INITCONFIG
{
	//int				nIndex;				//Connection index
	SOCKET			sockListener;		//Listen Socket
	//recv Ringbuffer size  = nRecvBufCnt * nRecvBufSize
	//int				nRecvBufCnt;
	//send Ringbuffer size  = nSendBufCnt * nSendBufSize
	//int				nSendBufCnt;
	int				nRecvBufSize;
	int				nSendBufSize;
	//int				nProcessPacketCnt;	//최대 처리 패킷의 개수
	int				nServerPort;
	int				nWorkerThreadCnt;	//i/o 처리를 위한 thread 개수
	//int				nProcessThreadCnt;	//패킷처리를 위한 thread 개수


	_INITCONFIG()
	{
		ZeroMemory(this, sizeof(INITCONFIG));
	}
}INITCONFIG;



// 여기 수정해주어야 함.소켓 정보 저장을 위한 구조체
struct EX_OVERLAPPED
{
	WSAOVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};



