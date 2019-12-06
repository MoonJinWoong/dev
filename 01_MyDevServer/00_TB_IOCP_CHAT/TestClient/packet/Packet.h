#pragma once
#include "PacketID.h"


namespace PacketLayer
{	
#pragma pack(push, 1)

	// ����ü�� ���� Ÿ������ �����غ���. ������
	
	struct RecvPacketInfo
	{
		int clientIdx = 0;
		short PacketId = 0;
		short PacketBodySize = 0;
		char* pRefData = 0;
	};
	// packet head 
	struct PktHeader
	{
		short TotalSize;
		short Id;
	};



	// ���� ���� �� 
	const int ERR_MSG_MAX = 30;
	struct SC_Error_Msg
	{
		char msg[ERR_MSG_MAX] = { 0, };
	};



	// �α��� ���� ����ü
	const int MAX_USER_ID_SIZE = 16;
	struct CS_Login_Pkt
	{
		char szID[MAX_USER_ID_SIZE+1] = { 0, };
	};
	struct SC_Login_Pkt
	{
		char msg[50] = { 0, };
	};




	//
	// �κ� ���� ����ü�� 
	//
	const int MAX_LOBBY_LIST_COUNT = 10;

	struct LobbyInfo
	{
		short LobbyID;
		short LobbyClientCount;
		short LobbyMaxClientCount;
	};

	struct CS_LobbyList_Pkt
	{

	};

	// ���� -> Ŭ�� : �κ� ������ �ѷ��ְ� 
	// ���� �κ� ���� ������ �����ش�.
	struct SC_LobbyList_Pkt
	{
		short LobbyCount = 0;
		LobbyInfo LobbyList[MAX_LOBBY_LIST_COUNT];
	};

	struct CS_LobbyEnter_Pkt
	{
		short lobbyIdBySelected = -1;
	};

	const int LOBBY_ENTER_MSG = 20;
	struct SC_LobbyEnter_Pkt
	{
		short MaxUserCount;
		short MaxRoomCount;
		char msg[LOBBY_ENTER_MSG+1] = { 0, };
	};

	const int MAX_LOBBY_CHAT_SIZE = 256;
	struct SC_Lobby_Chat_Pkt
	{
		char sendID[MAX_USER_ID_SIZE + 1] = { 0, };
		char msg[MAX_LOBBY_CHAT_SIZE + 1] = { 0, };
	};
	struct CS_Lobby_Chat_Pkt
	{
		char msg[MAX_LOBBY_CHAT_SIZE + 1] = { 0, };
	};


	
	// �� ���� 
	const int MAX_ROOM_MSG = 100;
	const int MAX_CHAT_MSG_SIZE = 256;
	struct ROOM_INFO
	{
		short mRoomId;
		short mInRoomUserCnt;   // ���� ��ȿ� �ִ� ����
		short mRoomMaxCnt;      // �� ��� �ο� �ִ�ġ
	};


	struct CS_ROOM_LIST_PKT
	{

	};

	struct SC_ROOM_LIST_PKT
	{
		short mRoomCount = 10;
		char mMsg[MAX_ROOM_MSG] = { 0, };
	};

	struct CS_ROOM_ENTER 
	{
		short RoomId = -1;
	};

	struct SC_ROOM_ENTER
	{
		char mMsg[MAX_ROOM_MSG] = { 0, };
	};
	struct CS_ROOM_CHAT
	{
		char msg[MAX_CHAT_MSG_SIZE + 1] = { 0, };
	};
	struct SC_ROOM_CHAT
	{
		char sendID[MAX_USER_ID_SIZE + 1] = { 0, };
		char msg[MAX_CHAT_MSG_SIZE + 1] = { 0, };
	};



#pragma pack(pop)


	
}