#include "ChatServer.h"
#include "../NetLib/easylogging++.h"
#include <iostream>

void ChatServer::ThrowLogicConnection(unsigned int unique_id,PACKET_TYPE type)
{
	mLogicProc->ConnectionPktData(unique_id,type);
}

void ChatServer::ThrowLogicRecv(CustomOverEx* pOver, unsigned int ioSize)
{
	RemoteSession* session = GetSessionByIdx(pOver->mUid);
	if (session == nullptr)
	{
		return;
	}

	if (ioSize != session->GetRecvBuffer().MoveWritePos(ioSize))
	{
		LOG(ERROR) << "ThrowLogicRecv::MoveWritePos";
		return;
	}

	PKT_HEADER header;
	while (session->GetRecvBuffer().GetReadAbleSize() > 0)
	{
		if (session->GetRecvBuffer().GetReadAbleSize() <= sizeof(header))
		{
			break;
		}

		session->GetRecvBuffer().GetHeaderSize((char*)&header, sizeof(header));

		if (session->GetRecvBuffer().GetReadAbleSize() < header.packet_len)
		{
			break;
		}
		else
		{
			mLogicProc->RecvPktData(session->GetUniqueId(), session->GetRecvBuffer().GetReadBufferPtr() , header.packet_len);
			session->RecvFinish(header.packet_len);
		}

	}
	session->RecvMsg();

}



void ChatServer::Run()
{
	// logic에서 Send를 부르면 실행
	auto SendFunc = [&](unsigned int index, int size, char* pPacket)
	{
		SendMsg(index, size, pPacket);
	};

	mLogicProc = std::make_unique<LogicMain>();
	mLogicProc->SendPacketFunc = SendFunc;
	mLogicProc->Init();
	mLogicProc->Start();
	StartNetService();
}

void ChatServer::Stop()
{
	mLogicProc->Stop();
	DestroyThread();
}

