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
	
	
	// 로그인 - 클라에서 서버로 보냄
	struct PktLogInReq
	{
		char szID[MAX_USER_ID_SIZE+1] = { 0, };
		//char szPW[MAX_USER_PASSWORD_SIZE+1] = { 0, };
	};
	// 로그인 - 서버에서 클라로 보냄
	struct PktLogInRes 
	{
		char msg[13] = { 0, };
	};


	// 로비 구조체
	const int MAX_LOBBY_COUNT = 20;   // 로비 최대 개수
	struct LobbyInfo
	{
		short LobbyId;
		short LobbyUserCount;
		short LobbyMaxUserCount;
	};

	// 로비 리스트 요청 - 클라에서 서버로 보냄
	struct PktLobbyListReq
	{

	};
	// 로비 리스트 응답 - 서버에서 클라로 보냄
	struct PktLobbyListRes
	{

	};

#pragma pack(pop)


	
}