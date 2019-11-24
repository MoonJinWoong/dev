#pragma once
#include "LogicMain.h"



void LogicMain::ProcConnect(u_Int uniqueId, int size, char* pData)
{
	auto Cli = mClMgr->GetClient(uniqueId);
	Cli->Reset();
}

void LogicMain::ProcDisConnect(u_Int uniqueId, int size, char* pData)
{
	//TODO ���� �������� �����ܿ��� ���� ó�� �ؾ���
}

void LogicMain::ProcLogin(u_Int uniqueId, int size, char* pData)
{
	auto packet = reinterpret_cast<CS_LOGIN_PKT*>(pData);
	auto inputId = packet->szID;

	//TODO: ���� - �����ڼ� �ʰ�
	//TODO: ���� - �̹� ���� 


	// �α��� ���� 
	auto client = mClMgr->GetClient(uniqueId);
	client->SetLogin((std::string)inputId);


	// ���� �޼���
	SC_LOGIN_PKT packet2;
	packet2.packet_type = (unsigned short)PACKET_TYPE::SC_LOGIN;
	packet2.packet_len  = sizeof(SC_LOGIN_PKT);
	strcpy_s(packet2.msg, "login sussess....!");

	SendPacketFunc(uniqueId, sizeof(SC_LOGIN_PKT), (char*)&packet2);
}

void LogicMain::ProcRoomEnter(u_Int uniqueId, int size, char* pData)
{
	SC_ROOM_ENTER packet;

}

