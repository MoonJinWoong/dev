#pragma once
#include <Windows.h>

struct PacketFrame
{
	unsigned int ClientIndex = 0;
	unsigned short PacketId = 0;
	unsigned short DataSize = 0;
	char* pDataPtr = nullptr;
};

enum class  PACKET_TYPE : unsigned short
{
	CONNECTION = 10,
	DISCONNECTION = 20,

};


