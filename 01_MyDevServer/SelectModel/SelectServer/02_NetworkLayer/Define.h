#pragma once


namespace NetworkLayer
{
	const int MAX_CLIENTS = 1000;



	const int MAX_IP_LEN = 32; // IP 문자열 최대 길이
	const int MAX_PACKET_BODY_SIZE = 1024; // 최대 패킷 보디 크기
	const int MaxClientRecvBufferSize = 8192;
	const int MaxClientSendBufferSize = 8192;

	const int MaxClientSockOptRecvBufferSize = 10240;
	const int MaxClientSockOptSendBufferSize = 10240;

	struct ClientsInfo
	{
		bool IsConnected() { return SocketFD != 0 ? true : false; }

		void Clear()
		{
			unique_id = 0;
			SocketFD = 0;
			IP[0] = '\0';
			RemainingDataSize = 0;
			PrevReadPosInRecvBuffer = 0;
			SendSize = 0;
		}

		int Index = 0;
		long long unique_id = 0;
		unsigned long long	SocketFD = 0;        // 소켓 번호 
		char    IP[MAX_IP_LEN] = { 0, };

		char* pRecvBuffer = nullptr;
		int     RemainingDataSize = 0;
		int     PrevReadPosInRecvBuffer = 0;

		char* pSendBuffer = nullptr;
		int     SendSize = 0;
	};

	struct RecvPacketInfo
	{
		int Index = 0;
		short PacketId = 0;
		short PacketBodySize = 0;
		char* pRefData = 0;
	};




#pragma pack(push, 1)

	// test packet
	struct testPacket
	{
		int UserID = 0;
		short Operation = 0;
		short bodySize = 0;
		char data[64];
	};


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
}