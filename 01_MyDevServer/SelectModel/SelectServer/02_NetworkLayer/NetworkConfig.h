#pragma once


namespace NetworkLib
{
	struct ServerConfig
	{
		unsigned short Port = 9000;
		int BackLogCount = 32;

		//int MaxClientCount;
		//int ExtraClientCount; // �����ϸ� �α��ο��� ¥������ MaxClientCount + �������� �غ��Ѵ�.

		//short MaxClientSockOptRecvBufferSize;
		//short MaxClientSockOptSendBufferSize;
		//short MaxClientRecvBufferSize;
		//short MaxClientSendBufferSize;

		//bool IsLoginCheck;	// ���� �� Ư�� �ð� �̳��� �α��� �Ϸ� ���� ����

		//int MaxLobbyCount;
		//int MaxLobbyUserCount;
		//int MaxRoomCountByLobby;
		//int MaxRoomUserCount;
	};


}