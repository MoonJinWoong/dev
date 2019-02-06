#pragma once


#define MAX_RECVBUFCNT					100    // iocp recv packet pool 에서 버퍼 개수
#define MAX_SENDBUFCNT					100
#define MAX_QUEUESIZE					10000 
#define MAX_QUEUESENDSIZE				1000
#define MAX_PBUFSIZE				    4096   // 패킷풀에서 버퍼 한개당 사이즈 
#define MAX_VPBUFSIZE					1024*40  // 가변 버퍼 사이즈 
#define MAX_RINGBUFSIZE                 1024*100
#define DEFAULT_BUFCOUNT                10       // 가변버퍼 개수 
#define PACKET_SIZE_LENGTH				4
#define PACKET_TYPE_LENGTH				2
#define MAX_IP_LENGTH					20	
#define MAX_PROCESS_THREAD				1	
#define MAX_WORKER_THREAD				10




enum eOperationType
{
	// work iocp operation 
	OP_SEND,
	OP_RECV,
	OP_ACCEPT,

	// process IOCP operation
	OP_CLOSE,       // 접종처리
	OP_RECVPACKET,  // 순서성 패킷 처리 
	OP_SYSTEM       // 시스템 MSG 처리 
};



// connection 설정을 위한 config 구조체 

typedef struct _INITCONFIG
{
	int nIndex; 
	SOCKET sockListener;
	
	// recv ringbuffer size = nRecvBufCnt * nRecvBufSize
	int nRecvBufferCnt;
	
	// send ringbuffer size = nSendBufCnt * nSendBufSize 
	int nSendBufCnt;
	int nRecvBufSize;
	int nSendBufSize;
	int nProcessPacketCnt;  // 최대 처리 패킷 개수 
	int nServerPort;
	int nWorkerTrheadCnt;
	int nProcessThreadCnt;

	_INITCONFIG()
	{
		ZeroMemory(this, sizeof(_INITCONFIG));
	}

}INITCONFIG;



typedef struct _OVERLAPPED_EX
{
	WSAOVERLAPPED			s_Overlapped;
	WSABUF					s_WsaBuf;
	int						s_nTotalBytes;
	DWORD					s_dwRemain;
	char*					s_lpSocketMsg;	// Send/Recv Buffer.
	eOperationType			s_eOperation;
	void*					s_lpConnection;
	_OVERLAPPED_EX(void* lpConnection)
	{
		ZeroMemory(this, sizeof(OVERLAPPED_EX));
		s_lpConnection = lpConnection;
	}
} OVERLAPPED_EX, *LPOVERLAPPED_EX;


typedef struct _PROCESSPACKET
{
	eOperationType	s_eOperationType;
	WPARAM			s_wParam;
	LPARAM			s_lParam;
	_PROCESSPACKET()
	{
		Init();
	}
	void Init()
	{
		ZeroMemory(this, sizeof(PROCESSPACKET));
	}

} PROCESSPACKET, *LPPROCESSPACKET;
