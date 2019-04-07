#include <algorithm>

#include "../NetworkLayer/SelectNetwork.h"
#include "Packet.h"
#include "ErrorSet.h"
#include "Player.h"
#include "Game.h"
#include "Room.h"

using PACKET_ID = LogicCommon::PACKET_ID;


namespace LogicLib
{
	Room::Room() {}
	Room::~Room()
	{
		if (m_pGame != nullptr) {
			delete m_pGame;
		}
	}
	void Room::Init(const short index, const short maxUserCount)
	{
		m_Index = index;
		m_MaxUserCount = maxUserCount;

		m_pGame = new Game;

	}

	void Room::SetNetwork(TcpNet* pNetwork)
	{
		m_pRefNetwork = pNetwork;
	}



}