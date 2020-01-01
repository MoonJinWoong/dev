#pragma once
#include "LogicMain.h"
#include <memory>



void LogicMain::ProcConnect(unsigned int uniqueId, unsigned int size, char* pData)
{
	std::cout << "[Connect]" << std::endl;
}

void LogicMain::ProcDisConnect(unsigned int uniqueId, unsigned int size, char* pData)
{
	std::cout << "[DisConnect]" << std::endl;
}

void LogicMain::ProcLogin(unsigned int uniqueId, unsigned int size, char* pData)
{

	auto packet = reinterpret_cast<CS_LOGIN_PKT*>(pData);
	auto inputId = packet->szID;

	SC_LOGIN_PKT packet2;
	packet2.packet_type = static_cast<unsigned short>(PACKET_TYPE::SC_LOGIN);
	packet2.packet_len = sizeof(SC_LOGIN_PKT);

	// overflow
	if (mClMgr->GetCurClientCnt() >= mClMgr->GetMaxClientCnt())
	{
		const char* retMsg = "login Full...!!!!";
		strcpy_s(packet2.msg, retMsg);
		SendPacketFunc(uniqueId, sizeof(SC_LOGIN_PKT), (char*)&packet2);
		return;
	}

	// already
	if (!mClMgr->FindID(inputId))
	{
		const char* retMsg = "login already...!!!!";
		strcpy_s(packet2.msg, retMsg);
		SendPacketFunc(uniqueId, sizeof(SC_LOGIN_PKT), (char*)&packet2);
	}
	// success
	else
	{

		// 로그인 세팅 
		mClMgr->IncreaseCurClientCnt();
		mClMgr->Add(inputId, uniqueId);

		// 성공 메세지
		strcpy_s(packet2.msg, "login sussess....!");
		SendPacketFunc(uniqueId, sizeof(SC_LOGIN_PKT), (char*)&packet2);
	}
}

void LogicMain::ProcRoomList(unsigned int uniqueId, unsigned int size, char* pData)
{
	SC_ROOM_LIST_PKT packet;
	packet.packet_type = static_cast<unsigned short>(PACKET_TYPE::SC_ROOM_LIST);
	packet.packet_len = sizeof(SC_ROOM_LIST_PKT);
	
	auto client = mClMgr->GetClient(uniqueId);

	if (client->GetUserState() != USER_STATE::LOGIN)
	{
		packet.mRoomCount = -1;
		strcpy_s(packet.mMsg, "You not login...!");
		SendPacketFunc(uniqueId, sizeof(SC_ROOM_LIST_PKT), (char*)&packet);
	}
	else
	{
		packet.mRoomCount = mMaxRoomCnt;
		strcpy_s(packet.mMsg, "success room list...!");
		SendPacketFunc(uniqueId, sizeof(SC_ROOM_LIST_PKT), (char*)&packet);
	}
}
void LogicMain::ProcRoomEnter(unsigned int uniqueId, unsigned int size, char* pData)
{
	std::cout << "RoomEnter" << std::endl;

	auto packet = reinterpret_cast<CS_ROOM_ENTER*>(pData);

	SC_ROOM_ENTER packet2;
	packet2.packet_type = static_cast<unsigned short>(PACKET_TYPE::SC_ROOM_ENTER);
	packet2.packet_len = sizeof(SC_ROOM_ENTER);


	auto client = mClMgr->GetClient(uniqueId);

	if (packet->RoomId == -1)
	{
		strcpy_s(packet2.mMsg, "you Request RoomList...!");
		SendPacketFunc(uniqueId, sizeof(SC_ROOM_ENTER), (char*)&packet2);
	}
	else if (!mRoomMgr->EnterRoom(packet->RoomId,client))
	{
		strcpy_s(packet2.mMsg, "room enter fail...!");
		SendPacketFunc(uniqueId, sizeof(SC_ROOM_ENTER), (char*)&packet2);
	}
	else
	{
		strcpy_s(packet2.mMsg, "room enter success...!");
		SendPacketFunc(uniqueId, sizeof(SC_ROOM_ENTER), (char*)&packet2);
	}
}
void LogicMain::ProcRoomChat(unsigned int uniqueId, unsigned int size, char* pData)
{
	auto recvPkt = reinterpret_cast<CS_ROOM_CHAT*>(pData);

	SC_ROOM_CHAT packet2;
	packet2.packet_type = static_cast<unsigned short>(PACKET_TYPE::SC_ROOM_CHAT);
	packet2.packet_len = sizeof(SC_ROOM_CHAT);

	auto client = mClMgr->GetClient(uniqueId);
	auto sendNickName = client->GetUserId();
	strcpy_s(packet2.sendID, sizeof(sendNickName.c_str() + 1), sendNickName.c_str());
	strcpy_s(packet2.msg, recvPkt->msg);

	auto roomNum = client->GetRoomIdx();
	if (roomNum != -1)
	{

	}

	auto room = mRoomMgr->GetRoomByNum(roomNum);

	for (auto cl : room->mClientList)
	{
		SendPacketFunc(cl->GetRemoteIdx(), sizeof(SC_ROOM_CHAT), (char*)&packet2);
	}
}

