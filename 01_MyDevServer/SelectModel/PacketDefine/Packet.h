#pragma once

#include "PacketID.h"


namespace PacketLayer
{	
#pragma pack(push, 1)

	
	struct RecvPacketInfo
	{
		int SessionIndex = 0;
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
	struct PktLogInReq
	{
		char szID[MAX_USER_ID_SIZE+1] = { 0, };
		//char szPW[MAX_USER_PASSWORD_SIZE+1] = { 0, };
	};
	// �α��� - �������� Ŭ��� ����
	struct PktLogInRes 
	{
		char msg[13] = { 0, };
	};


	// �κ� ����ü
	const int MAX_LOBBY_COUNT = 20;   // �κ� �ִ� ����
	struct LobbyInfo
	{
		short LobbyId;
		short LobbyUserCount;
		short LobbyMaxUserCount;
	};

	// �κ� ����Ʈ ��û - Ŭ�󿡼� ������ ����
	struct PktLobbyListReq
	{

	};
	// �κ� ����Ʈ ���� - �������� Ŭ��� ����
	struct PktLobbyListRes
	{

	};

#pragma pack(pop)


	
}