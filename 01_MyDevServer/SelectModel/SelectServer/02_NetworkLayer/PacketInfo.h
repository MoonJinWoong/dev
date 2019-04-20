#pragma once


#pragma push(1)
struct Packet
{
	int UserID = 0;
	short Operation = 0;
	short bodySize = 0;
	char data[512];
};
#pragma pop

struct RecvPacketInfo
{
	int SessionIndex = 0;
	short PacketId = 0;
	short PacketBodySize = 0;
	char* pRefData = 0;
};