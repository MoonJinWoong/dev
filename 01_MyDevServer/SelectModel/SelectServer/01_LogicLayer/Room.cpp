#include <algorithm>

#include "../02_NetworkLayer/SelectNetwork.h"
#include "../../PacketDefine/Packet.h"
#include "LogicErrorSet.h"

#include "Client.h"
#include "InGame.h"
#include "Room.h"


namespace LogicLayer
{
	Room::Room() {}
	Room::~Room(){}
	void Room::Init(const short index, const short maxClientCount)
	{
		m_RoomIndex = index;
		m_MaxClientCnt = maxClientCount;

		m_pGame = new InGame();
	}
	void Room::SetNetworkRoom(SelectNet* pNetwork)
	{
		m_pRefNetwork = pNetwork;
	}
}