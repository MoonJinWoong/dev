#pragma once
#include <Windows.h>
#include "TypeDefine.h"


struct PacketFrame
{
	u_Int unique_id = 0;
	u_Short packet_type = 0;
	u_Short size = 0;
	char* pData = nullptr;
};

enum class  PACKET_TYPE : u_Short
{
	NONE = 0,
	CONNECTION = 1,
	DISCONNECTION = 2,

	/* SC -> server to client */
	/* CS -> client to server */
	CS_LOGIN = 100,
	SC_LOGIN = 101,
};



/* Client class�� ��� */
#pragma pack(push,1)
struct PKT_HEADER
{
	u_Short packet_len;
	u_Short packet_type;
};

c_u_Int PKT_HEADER_LENGTH = sizeof(PKT_HEADER);


// �α��� ���� ����ü
c_Int MAX_USER_ID_SIZE = 16;

struct CS_LOGIN_PKT : public PKT_HEADER
{
	char szID[MAX_USER_ID_SIZE + 1] = { 0, };
};
struct SC_LOGIN_PKT : public PKT_HEADER
{
	char msg[50] = { 0, };
};


#pragma pack(pop) //���� ������ ��ŷ������ �����



//
//#pragma once
//#include "PacketID.h"
//
//
//namespace PacketLayer
//{
//#pragma pack(push, 1)
//
//	// ����ü�� ���� Ÿ������ �����غ���. ������
//
//	struct RecvPacketInfo
//	{
//		int clientIdx = 0;
//		short PacketId = 0;
//		short PacketBodySize = 0;
//		char* pRefData = 0;
//	};
//	// packet head 
//	struct PktHeader
//	{
//		short TotalSize;
//		short Id;
//	};
//
//
//
//	// ���� ���� �� 
//	const int ERR_MSG_MAX = 30;
//	struct SC_Error_Msg
//	{
//		char msg[ERR_MSG_MAX] = { 0, };
//	};
//
//
//
//	// �α��� ���� ����ü
//	const int MAX_USER_ID_SIZE = 16;
//	struct CS_Login_Pkt
//	{
//		char szID[MAX_USER_ID_SIZE + 1] = { 0, };
//	};
//	struct SC_Login_Pkt
//	{
//		char msg[13] = { 0, };
//	};
//
//
//
//
//	//
//	// �κ� ���� ����ü�� 
//	//
//	const int MAX_LOBBY_LIST_COUNT = 10;
//
//	struct LobbyInfo
//	{
//		short LobbyID;
//		short LobbyClientCount;
//		short LobbyMaxClientCount;
//	};
//
//	struct CS_LobbyList_Pkt
//	{
//	};
//
//	// ���� -> Ŭ�� : �κ� ������ �ѷ��ְ� 
//	// ���� �κ� ���� ������ �����ش�.
//	struct SC_LobbyList_Pkt
//	{
//		short LobbyCount = 0;
//		LobbyInfo LobbyList[MAX_LOBBY_LIST_COUNT];
//	};
//
//	struct CS_LobbyEnter_Pkt
//	{
//		short lobbyIdBySelected = -1;
//	};
//
//	const int LOBBY_ENTER_MSG = 20;
//	struct SC_LobbyEnter_Pkt
//	{
//		short MaxUserCount;
//		short MaxRoomCount;
//		char msg[LOBBY_ENTER_MSG + 1] = { 0, };
//	};
//
//	const int MAX_LOBBY_CHAT_SIZE = 256;
//	struct SC_Lobby_Chat_Pkt
//	{
//		char sendID[MAX_USER_ID_SIZE + 1] = { 0, };
//		char msg[MAX_LOBBY_CHAT_SIZE + 1] = { 0, };
//	};
//	struct CS_Lobby_Chat_Pkt
//	{
//		char msg[MAX_LOBBY_CHAT_SIZE + 1] = { 0, };
//	};
//
//
//
//	//
//	// �뿡 ���� ����ü
//	//
//	const int ROOM_ENTER_MSG = 20;
//	struct SC_Room_Enter
//	{
//		char ID[MAX_USER_ID_SIZE] = { 0, };
//	};
//	struct CS_Room_Enter
//	{
//		short roomIdBySelected;
//	};
//
//
//
//
//#pragma pack(pop)
//
//
//
//}