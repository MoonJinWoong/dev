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

