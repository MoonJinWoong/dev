#include "ChatServer.h"
#include <iostream>

void ChatServer::ThrowLogicConnection(unsigned int unique_id,PACKET_TYPE type)
{
	mLogicProc->ConnectionPktData(unique_id,type);
}

void ChatServer::ThrowLogicRecv(CustomOverEx *pOver, unsigned int ioSize)
{
	RemoteSession* session = GetSessionByIdx(pOver->mUid);
	if (session == nullptr)
	{
		return;
	}
	pOver->mRemainByte += ioSize;
	auto& remain = pOver->mRemainByte;

	auto pBuf = pOver->mBuf.data();

	// ��Ŷ ��� ����, ��Ŷ ��ü ����
	const int PKT_HEAD_LEN = sizeof(PKT_HEADER);
	const int PKT_TOTAL_LEN = 2;

	short packetSize = 0;

	while (true)
	{
		if (remain < PKT_HEAD_LEN)
		{
			break;
		}
		// ��Ŷ ������� ��Ż ���� ����
		CopyMemory(&packetSize, pBuf, PKT_TOTAL_LEN);

		if (packetSize <= 0 || packetSize > MAX_SOCKBUF)
		{
			return;
		}


		// �������� ������.
		if (remain >= (DWORD)packetSize)
		{
			mLogicProc->RecvPktData(session->GetUniqueId(), pBuf , packetSize);
			remain -= packetSize;
			pBuf += packetSize;
		}
		else
		{
			break;
		}
	}

	session->mRecvOverEx.mRemainByte = remain;
}



void ChatServer::Run(unsigned int maxClient)
{
	// logic���� Send�� �θ��� ����
	auto SendFunc = [&](unsigned int index, int size, char* pPacket)
	{
		SendMsg(index, size, pPacket);
	};

	mLogicProc = std::make_unique<LogicMain>();
	mLogicProc->SendPacketFunc = SendFunc;
	mLogicProc->Init(maxClient);
	mLogicProc->Start();
	StartNetService(maxClient);
}

void ChatServer::Stop()
{
	mLogicProc->Stop();
	DestroyThread();
}

