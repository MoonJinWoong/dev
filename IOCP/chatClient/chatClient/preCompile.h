#pragma once

//�̰����� ���հ���


#pragma warning( disable:4786 )
#pragma warning( disable:4251 )
#pragma warning( disable:4311 )
#pragma warning( disable:4244 )
#pragma warning( disable:4996 )

const int SERVER_PORT = 9000;
const int MAX_WORKER_THREAD = 2;
const int MAX_BUFF_SIZE = 1024;
const int MAX_PACKET_SIZE = 255;



// Protocol ID
const int  CS_MSG = 1;
const int  SC_MSG = 1;



#pragma comment(lib,"Ws2_32.lib")


#include <iostream>
#include <WinSock2.h>
#include <winsock.h>
#include <thread>
#include <Windows.h>


using namespace std;



// �̺�Ʈ

enum EVENTTYPE { E_RECV, E_SEND, E_BROAD_CAST };

// �۾��� ���� ������ �����Ǿ�� �ϱ� ������ �������� ��. ���÷� �θ� ���´�. 
struct WSAOVERLAPPED_EX
{
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	char IOCP_buf[MAX_BUFF_SIZE];
	EVENTTYPE event_type;
};




//Ŭ���̾�Ʈ ������ ������� ����ü
struct sClientInfo {
	SOCKET				sSocketClient;			//Cliet�� ����Ǵ� ����
	WSAOVERLAPPED_EX	sRecvOverlappedEx;	//RECV Overlapped I/O�۾��� ���� ����
	WSAOVERLAPPED_EX	sSendOverlappedEx;	//SEND Overlapped I/O�۾��� ���� ����
	//�����ڿ��� ��� �������� �ʱ�ȭ
	sClientInfo()
	{
		ZeroMemory(&sRecvOverlappedEx, sizeof(WSAOVERLAPPED_EX));
		ZeroMemory(&sSendOverlappedEx, sizeof(WSAOVERLAPPED_EX));
		sSocketClient = INVALID_SOCKET;
	}
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

