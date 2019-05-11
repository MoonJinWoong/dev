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
	
	
	// 로그인 - 클라에서 서버로 보냄
	struct CS_Login_Pkt
	{
		char szID[MAX_USER_ID_SIZE+1] = { 0, };
	};
	// 로그인 - 서버에서 클라로 보냄
	struct SC_Login_Pkt
	{
		char msg[13] = { 0, };
	};

	// 로비 리스트 
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

	// 서버 -> 클라 : 로비 갯수를 뿌려주고 
	// 각각 로비에 대한 정보를 던져준다.
	struct SC_LobbyList_Pkt
	{
		short LobbyCount = 0;
		LobbyInfo LobbyList[MAX_LOBBY_LIST_COUNT];
	};




#pragma pack(pop)


	
}