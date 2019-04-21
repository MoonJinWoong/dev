#pragma once


// 4πŸ¿Ã∆Æ 
struct Head
{
	int id;
};

struct Body
{
	char buf[512 + 1];
};

#pragma push(1)
struct Packet
{
	int UserID = 0;
	short Operation = 0;
	short bodySize = 0;
	char data[64];
};

struct SendPacketInfo
{
	int Index = 0;
	short PacketId = 0;
	short PacketBodySize = 0;
	char* pRefData = 0;
};

#pragma pack(push, 1)
struct PacketHead
{
	short TotalSize;
	short Id;
	unsigned char* Reserve;
};

const int PACKET_HEADER_SIZE = sizeof(PacketHead);


struct PktNtfSysCloseSession : PacketHead
{
	int SockFD;
};
#pragma pack(pop)
#pragma pop