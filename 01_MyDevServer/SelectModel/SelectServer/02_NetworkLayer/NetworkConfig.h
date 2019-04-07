#pragma once


namespace NetworkLib
{
	struct ServerConfig
	{
		unsigned short Port = 9000;
		int BackLogCount = 32;

		//int MaxClientCount;
		//int ExtraClientCount; // 가능하면 로그인에서 짜르도록 MaxClientCount + 여유분을 준비한다.

		//short MaxClientSockOptRecvBufferSize;
		//short MaxClientSockOptSendBufferSize;
		//short MaxClientRecvBufferSize;
		//short MaxClientSendBufferSize;

		//bool IsLoginCheck;	// 연결 후 특정 시간 이내에 로그인 완료 여부 조사

		//int MaxLobbyCount;
		//int MaxLobbyUserCount;
		//int MaxRoomCountByLobby;
		//int MaxRoomUserCount;
	};


}