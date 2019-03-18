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
	//int				nProcessPacketCnt;	//�ִ� ó�� ��Ŷ�� ����
	int				nServerPort;
	int				nWorkerThreadCnt;	//i/o ó���� ���� thread ����
	//int				nProcessThreadCnt;	//��Ŷó���� ���� thread ����


	_INITCONFIG()
	{
		ZeroMemory(this, sizeof(INITCONFIG));
	}
}INITCONFIG;



// ���� �������־�� ��.���� ���� ������ ���� ����ü
struct EX_OVERLAPPED
{
	WSAOVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
};



