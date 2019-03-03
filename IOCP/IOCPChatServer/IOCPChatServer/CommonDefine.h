#pragma once



// define ��ſ� const�� ���(������ ���� �ٽ� ���� �� ��)
const int MAX_WORKER_THREAD = 8;
const int MAX_PROCESS_TRHEAD = 1;


const int PACKET_SIZE_LENGTH = 4;	//

// CircleBuffersize 
const int MAX_CIRCLE_BUFFER_SIZE = 1024 * 100;

const BYTE MAX_IP_LENGTH = 20;

enum OperationType
{
	//Work IOCP operation
	OP_SEND,
	OP_RECV,
	OP_ACCEPT,


	//Process IOCP operation
	OP_CLOSE,		//���� ����ó��
	OP_RECVPACKET,	//���� �� ��Ŷ ó��
	OP_SYSTEM		//�ý��� �޽��� ó��
};

typedef struct INITCONFIG
{
	//  member ���� ���̹��� ����ü �ձ��� ���� s�� 

	INITCONFIG()
	{
		ZeroMemory(this, sizeof(INITCONFIG));
	}

	DWORD				sIndex;				//Connection index
	SOCKET			sListenSocket;		//Listen Socket
	//recv Ringbuffer size  = sRecvBufCnt * sRecvBufSize
	DWORD				sRecvBufCnt;
	//send Ringbuffer size  = sSendBufCnt * sSendBufSize
	DWORD				sSendBufCnt;
	DWORD				sRecvBufSize;
	DWORD				sSendBufSize;
	DWORD				sProcessPacketCnt;	//�ִ� ó�� ��Ŷ�� ����
	DWORD				sServerPort;
	DWORD				sWorkerThreadCnt;	//i/o ó���� ���� thread ����
	DWORD				sProcessThreadCnt;	//��Ŷó���� ���� thread ����

}INITCONFIG;


typedef struct OVERLAPPED_EX
{
	WSAOVERLAPPED			sOverlapped;
	WSABUF					sWsaBuf;
	int						sTotalBytes;
	DWORD					sDwRemain;
	char*					slpSocketMsg;	// Send/Recv Buffer.
	OperationType			sOperation;
	void*					slpConnection;
	OVERLAPPED_EX(void* lpConnection)
	{
		ZeroMemory(this, sizeof(OVERLAPPED_EX));
		slpConnection = lpConnection;
	}
} OVERLAPPED_EX, *LPOVERLAPPED_EX;