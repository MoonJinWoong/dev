
#include <memory>

// ���� ���� ��������ٰ� ��Ŭ������־����

#include "../NetworkLayer/Defines.h"
#include "../NetworkLayer/SelectNetwork.h"
#include "LogicMain.h"
#include <iostream>

#pragma comment(lib , "Debug/NetworkLayer.lib")


namespace LogicLib
{
	LogicMain::LogicMain() { std::cout << "�������� ������" << std::endl; }
	LogicMain::~LogicMain()
	{
		//Release();
	}
	bool LogicMain::Init()
	{
		// config setting
		NetworkLayer::ServerConfig config;
		
		config.Port = 9000;

		// NetworkLayer -> SelectNetwork�� �ʱ�ȭ 
		m_pNetwork = std::make_unique<NetworkLayer::SelectNetwork>();
		if(m_pNetwork->Init(config) == false) std::cout << "���� ���� ini ����.." << std::endl;

		
		return true;
	}

}
