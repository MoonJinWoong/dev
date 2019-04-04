#pragma once

namespace NetworkLib
{
	struct ServerConfig
	{
		unsigned short Port = 9000;
		int BackLogCount;
		int MaxClientCount;
		int ExtraClientCount;  // 클라 짤라줄 변수 


		short MaxClientRecvBufferSize;
		short MaxClientSendBufferSize;
	};


	// define 보다 const int 이게 더 성능이 좋음
	// 단점은 변수를 이용할 때 색이 안변한다 ^오^


	const int MAX_IP_LEN = 32; // IP 문자열 최대 길이
	const int MAX_PACKET_BODY_SIZE = 1024; // 최대 패킷 보디 크기

	struct ClientSession
	{
		bool IsConnected() { return SocketFD != 0 ? true : false; }

		void Clear()
		{
			Seq = 0;
			SocketFD = 0;
			IP[0] = '\0';
			RemainingDataSize = 0;
			PrevReadPosInRecvBuffer = 0;
			SendSize = 0;
		}

		int Index = 0;
		long long Seq = 0;
		unsigned long long	SocketFD = 0;
		char    IP[MAX_IP_LEN] = { 0, };

		char*   pRecvBuffer = nullptr;
		int     RemainingDataSize = 0;
		int     PrevReadPosInRecvBuffer = 0;

		char*   pSendBuffer = nullptr;
		int     SendSize = 0;
	};
}