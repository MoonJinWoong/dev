#pragma once
#include <Windows.h>

struct PacketFrame
{
	unsigned int unique_id = 0;
	unsigned short packet_type = 0;
	unsigned short size = 0;
	char* pData = nullptr;
};

enum class  PACKET_TYPE : unsigned short
{
	NONE = 0,
	CONNECTION = 1,
	DISCONNECTION = 2,  // ����

	/* SC -> server to client */
	/* CS -> client to server */
	SC_LOGIN = 100,
	CS_LOGIN = 101,
};



/* Client class�� ��� */
#pragma pack(push,1)
struct PACKET_HEADER
{
	unsigned short packet_len;
	unsigned short packet_type;
	//UINT8 Type; //���࿩�� ��ȣȭ���� �� �Ӽ��� �˾Ƴ��� ��
};

const unsigned int PACKET_HEADER_LENGTH = sizeof(PACKET_HEADER);





#pragma pack(pop) //���� ������ ��ŷ������ �����
