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
	DISCONNECTION = 2,  // 기준

	/* SC -> server to client */
	/* CS -> client to server */
	SC_LOGIN = 100,
	CS_LOGIN = 101,
};



/* Client class가 사용 */
#pragma pack(push,1)
struct PACKET_HEADER
{
	unsigned short packet_len;
	unsigned short packet_type;
	//UINT8 Type; //압축여부 암호화여부 등 속성을 알아내는 값
};

const unsigned int PACKET_HEADER_LENGTH = sizeof(PACKET_HEADER);





#pragma pack(pop) //위에 설정된 패킹설정이 사라짐
