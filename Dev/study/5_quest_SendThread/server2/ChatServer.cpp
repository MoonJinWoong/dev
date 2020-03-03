#include "ChatServer.h"
#include <iostream>

void ChatServer::OnAccept(unsigned int unique_id)
{
	// 접속할때마다 큐에 밀어넣는다.
	auto id = unique_id;
	std::cout << "[OnAccept] UniqueID : " << id << std::endl;
	PacketFrame packet{ id ,(int)PACKET_TYPE::CONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
}

void ChatServer::OnClose(unsigned int unique_id)
{
	// 세션이 접속끊을때마다 큐에 밀어넣는다.
	PacketFrame packet{ unique_id ,(int)PACKET_TYPE::DISCONNECTION,0 };
	mLogicProc->PutConnectPkt(packet);
}

void ChatServer::OnRecv(CustomOverEx *pOver, unsigned int ioSize)
{
	RemoteSession* session = GetSessionByIdx(pOver->mUid);
	if (session == nullptr)
	{
		return;
	}
	pOver->mWSABuf.buf = pOver->mBuf.data();
	pOver->mRemainByte += ioSize;
	
	auto& remain = pOver->mRemainByte;
	auto pBuf = pOver->mWSABuf.buf;

	// 패킷 헤더 길이, 패킷 전체 길이
	const int PKT_HEAD_LEN = sizeof(PKT_HEADER);
	const int PKT_TOTAL_LEN = 2;
	const int PKT_TYPE_LEN = 2;

	short packetSize = 0;

	while (true)
	{
		if (remain < PKT_HEAD_LEN)
		{
			break;
		}
		// 패킷 헤더에서 토탈 길이 추출
		CopyMemory(&packetSize, pBuf, PKT_TOTAL_LEN);

		if (packetSize <= 0 || packetSize > MAX_SOCKBUF)
		{
			return;
		}


		// 로직으로 던진다.
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

	//session->mRecvOverEx.mBuf.fill(0);
}



void ChatServer::Run(unsigned int maxClient)
{
	// logic에서 Send를 부르면 실행
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

