#pragma once

namespace NetworkLib
{
	struct ServerConfig
	{
		unsigned short Port = 9000;
		int BackLogCount;
		int MaxClientCount;
		int ExtraClientCount;  // Ŭ�� ©���� ���� 


		short MaxClientRecvBufferSize;
		short MaxClientSendBufferSize;
	};


	// define ���� const int �̰� �� ������ ����
	// ������ ������ �̿��� �� ���� �Ⱥ��Ѵ� ^��^


	const int MAX_IP_LEN = 32; // IP ���ڿ� �ִ� ����
	const int MAX_PACKET_BODY_SIZE = 1024; // �ִ� ��Ŷ ���� ũ��

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