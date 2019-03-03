#pragma once

const int SERVER_PORT =  9000;
const int MAX_WORKER_THREAD = 2;
const int MAX_BUFF_SIZE = 1024;
const int MAX_PACKET_SIZE = 255;



// Protocol ID
const int  CS_MSG = 1;
const int  SC_MSG = 1;



// �̺�Ʈ

enum EVENTTYPE { E_RECV, E_SEND, E_BROAD_CAST };


//Connection������ ���� config����ü
typedef struct _INITCONFIG
{
	int				nIndex;				//Connection index
	SOCKET			sockListener;		//Listen Socket
	//recv Ringbuffer size  = nRecvBufCnt * nRecvBufSize
	int				sRecvBufCnt;
	//send Ringbuffer size  = nSendBufCnt * nSendBufSize
	int				sSendBufCnt;
	int				sRecvBufSize;
	int				sSendBufSize;
	int				sProcessPacketCnt;	//�ִ� ó�� ��Ŷ�� ����
	int				sServerPort;
	int				sWorkerThreadCnt;	//i/o ó���� ���� thread ����
	int				sProcessThreadCnt;	//��Ŷó���� ���� thread ����

	_INITCONFIG()
	{
		ZeroMemory(this, sizeof(INITCONFIG));
	}
}INITCONFIG;



// �۾��� ���� ������ �����Ǿ�� �ϱ� ������ �������� ��. ���÷� �θ� ���´�. 
struct WSAOVERLAPPED_EX
{
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	EVENTTYPE event_type;
};



#pragma pack (push, 1)    // ����ü ������ ����

struct cs_packet_msg
{
	BYTE size;
	BYTE type;
};

// �������� Ŭ�� ����ü ����
struct sc_packet_msg 
{
	BYTE size;
	BYTE type;
};

#pragma pack(pop)
