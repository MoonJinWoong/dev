#pragma once
#include <Windows.h>

struct PacketFrame
{
	unsigned int ClientIndex = 0;
	unsigned short PacketId = 0;
	unsigned short DataSize = 0;
	char* pDataPtr = nullptr;
};



