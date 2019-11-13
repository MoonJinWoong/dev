#pragma once
#include "LogicMain.h"



void LogicMain::ProcConnect(u_Int uniqueId, int size, char* pData)
{
	std::cout << "LogicMain ProcConnect" << std::endl;
}

void LogicMain::ProcDisConnect(u_Int uniqueId, int size, char* pData)
{
	std::cout << "LogicMain ProcDisConnect" << std::endl;
}

void LogicMain::ProcLogin(u_Int uniqueId, int size, char* pData)
{
	auto packet = reinterpret_cast<CS_LOGIN_PKT*>(pData);
	auto inputId = packet->szID;

	//TODO: 예외 - 접속자수 초과

	//TODO: 예외 - 이미 접속 
	

	// 로그인 세팅 
	auto client = mClMgr->GetClient(uniqueId);
	client->SetLogin((std::string)inputId);


	// 성공 메세지
	SC_LOGIN_PKT packet2;
	packet2.packet_type = (unsigned short)PACKET_TYPE::SC_LOGIN;
	packet2.packet_len  = sizeof(SC_LOGIN_PKT);
	strcpy_s(packet2.msg, "login sussess....!");

	SendPacketFunc(uniqueId, sizeof(SC_LOGIN_PKT), (char*)&packet2);
}

void LogicMain::ProcLobbyList(u_Int uniqueId, int size, char* pData)
{
	SC_LOBBY_LIST_PKT packet;

	// 로비 매니저에서 로비 갯수 가져옴
	packet.LobbyCount = 10;

	//TODO : 로비랑 로비매니저 만들기

}

