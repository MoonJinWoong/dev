#include <thread>
#include "../NetworkLibrary/Define.h"
#include "../NetworkLibrary/selectNetwork.h"
#include "LogicMain.h"



namespace ChatLogic 
{
	LogicMain::LogicMain() {}
	LogicMain::~LogicMain() {}

	bool LogicMain::Init()
	{
		NetworkLib::ServerOption m_ServerConfig;

		m_ServerConfig.Port = 9000;
		m_ServerConfig.MaxClientCount = 500;
		m_ServerConfig.ExtraClientCount = 100;

		m_ServerConfig.MaxClientSockOptRecvBufferSize = 512;
		m_ServerConfig.MaxClientSockOptSendBufferSize = 512;
		m_ServerConfig.MaxClientRecvBufferSize = 512;
		m_ServerConfig.MaxClientSendBufferSize = 512;

		m_ServerConfig.IsLoginCheck = 0;

		m_ServerConfig.MaxLobbyCount = 2;	// 로비 총 개수 
		m_ServerConfig.MaxLobbyUserCount = 50; 		// 로비 하나당 유저 최대수
		m_ServerConfig.MaxRoomCountByLobby = 20;
		m_ServerConfig.MaxRoomUserCount = 4;

		
		m_pNetwork = std::make_unique<NetworkLib::SelectNetwork>();
		m_pNetwork->Init(m_ServerConfig);


		//m_pNetObj = std::make_unique<NetworkLib::SelectNetwork>();
		return true;
	}

	void LogicMain::Start()
	{

	}

	void LogicMain::Stop()
	{

	}
}
