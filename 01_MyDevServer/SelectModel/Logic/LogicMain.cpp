
#include <memory>

// 로직 메인 헤더위에다가 인클루드해주어야함

#include "../NetworkLayer/Defines.h"
#include "../NetworkLayer/SelectNetwork.h"
#include "LogicMain.h"
#include <iostream>

#pragma comment(lib , "Debug/NetworkLayer.lib")


namespace LogicLib
{
	LogicMain::LogicMain() { std::cout << "로직메인 생성자" << std::endl; }
	LogicMain::~LogicMain()
	{
		//Release();
	}
	bool LogicMain::Init()
	{
		// config setting
		NetworkLayer::ServerConfig config;
		
		config.Port = 9000;

		// NetworkLayer -> SelectNetwork로 초기화 
		m_pNetwork = std::make_unique<NetworkLayer::SelectNetwork>();
		if(m_pNetwork->Init(config) == false) std::cout << "로직 계층 ini 실패.." << std::endl;

		
		return true;
	}

}
