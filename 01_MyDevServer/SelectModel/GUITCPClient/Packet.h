#pragma once


// 4����Ʈ 
struct Head
{
	int id;
};

struct Body
{
	char buf[512 + 1];
};

#pragma pack(push,1)
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

struct PacketHead
{
	short TotalSize;
	short Id;
};


//- �α��� ��û
const int MAX_USER_ID_SIZE = 16;
const int MAX_USER_PASSWORD_SIZE = 16;

// Ŭ�� �������� ������ ��Ŷ
struct Packet_Login_CtoS
{
	char szID[MAX_USER_ID_SIZE + 1] = { 0, };
	char szPW[MAX_USER_PASSWORD_SIZE + 1] = { 0, };
};

// ������ Ŭ������ ������ ��Ŷ
struct Packet_Login_ServerToClient
{

};

const int PACKET_HEADER_SIZE = sizeof(PacketHead);


struct PktNtfSysCloseSession : PacketHead
{
	int SockFD;
};
#pragma pack(pop)
