#include "ChatServer.h"
#include "../NetLib/easylogging++.h"
#include <iostream>

void ChatServer::PostLogicConnection(unsigned int unique_id,PACKET_TYPE type)
{
	mLogicProc->ConnectionPktData(unique_id,type);
}

void ChatServer::PostLogicRecv(unsigned int unique_id , char* pData, unsigned int packetLen)
{
	mLogicProc->RecvPktData(unique_id, pData, packetLen);
}

void ChatServer::Run()
{
	// logic에서 Send를 부르면 실행
	auto SendFunc = [&](unsigned int index, int size, char* pPacket)
	{
		NetService::SendMsg(index, size, pPacket);
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

