#pragma once
#include "PacketID.h"


namespace PacketLayer
{	
#pragma pack(push, 1)

	
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

	// packet body 
	const int MAX_USER_ID_SIZE = 16;
	const int MAX_USER_PASSWORD_SIZE = 16;
	
	
	// �α��� - Ŭ�󿡼� ������ ����
	struct CS_Login_Pkt
	{
		char szID[MAX_USER_ID_SIZE+1] = { 0, };
	};
	// �α��� - �������� Ŭ��� ����
	struct SC_Login_Pkt
	{
		char msg[13] = { 0, };
	};

	// �κ� ����Ʈ 
	const int MAX_LOBBY_LIST_COUNT = 10;

	struct LobbyInfo
	{
		short LobbyID;
		short LobbyUserCount;
		short LobbyMaxUserCount;
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




#pragma pack(pop)


	
}