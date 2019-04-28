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
	

	// Ŭ�󿡼� ������ ����
	struct PktLogInReq
	{
		char szID[MAX_USER_ID_SIZE+1] = { 0, };
		//char szPW[MAX_USER_PASSWORD_SIZE+1] = { 0, };
	};

	// �������� Ŭ��� ����
	struct PktLogInRes 
	{
		char msg[11] = { 0, };
	};

#pragma pack(pop)


	
}