#pragma once
#include "LogicMain.h"



void LogicMain::ProcConnect(u_Int uniqueId, u_Short size, char* pData)
{
	std::cout << "LogicMain ProcConnect" << std::endl;
}

void LogicMain::ProcDisConnect(u_Int uniqueId, u_Short size, char* pData)
{
	std::cout << "LogicMain ProcDisConnect" << std::endl;
}

void LogicMain::ProcLogin(u_Int uniqueId, u_Short size, char* pData)
{
	auto packet = reinterpret_cast<CS_LOGIN_PKT*>(pData);
	auto inputId = packet->szID;

	//TODO: 예외 - 접속자수 초과

	//TODO: 예외 - 이미 접속 
	
	
	
	// 성공 메세지 보낸다
	SC_LOGIN_PKT packet2;
	packet2.packet_type = (unsigned short)PACKET_TYPE::SC_LOGIN;
	packet2.packet_len  = sizeof(SC_LOGIN_PKT);
	strcpy_s(packet2.msg, "login sussess....!");

	SendPacketFunc(uniqueId, sizeof(SC_LOGIN_PKT), (char*)&packet2);
}


